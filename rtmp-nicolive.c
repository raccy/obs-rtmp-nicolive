#include <stdbool.h>
#include <obs-module.h>
#include "nicolive-log.h"
#include "nicolive.h"

// use in rtmp_nicolive_update_internal for reset default settigs
#define reset_obs_data(type, settings, name) \
	obs_data_set_##type(                 \
	    (settings), (name), obs_data_get_##type((settings), (name)))

enum rtmp_nicolive_login_type {
	RTMP_NICOLIVE_LOGIN_MAIL,
	RTMP_NICOLIVE_LOGIN_SESSION,
	RTMP_NICOLIVE_LOGIN_VIQO,
};

static inline bool adjust_bitrate(long long bitrate,
    obs_data_t *video_encoder_settings, obs_data_t *audio_encoder_settings)
{
	if (bitrate <= 0) {
		nicolive_log_warn("total bitrate is zero or negative");
		return false;
	}

	long long video_bitrate =
	    obs_data_get_int(video_encoder_settings, "bitrate");
	long long audio_bitrate =
	    obs_data_get_int(audio_encoder_settings, "bitrate");
	nicolive_log_debug("video bitrate: %lld", video_bitrate);
	nicolive_log_debug("audio bitrate: %lld", audio_bitrate);

	// FIXME: audio 0 ... bug?
	if (audio_bitrate <= 0) {
		nicolive_log_warn("audo bitrate is zero or negative");
		return false;
	}

	long long adjust_bitrate = bitrate - audio_bitrate;

	// the smallest video bitrate is ...
	if (adjust_bitrate < 0) {
		nicolive_msg_warn(true,
		    obs_module_text("MessageFailedAdjustBitrate"),
		    "audio bitrate is too high");
		return false;
	}

	if (adjust_bitrate != video_bitrate) {
		obs_data_set_int(
		    video_encoder_settings, "bitrate", adjust_bitrate);
		nicolive_log_debug(
		    "adjust video bitrate: %lld", adjust_bitrate);
	} else {
		nicolive_log_debug("need not to adjust video bitrate");
	}
	return true;
}

static void rtmp_nicolive_apply_encoder_settings(void *data,
    obs_data_t *video_encoder_settings, obs_data_t *audio_encoder_settings);

static const char *rtmp_nicolive_getname(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return obs_module_text("NiconicoLive");
}

static void rtmp_nicolive_update_internal(
    void *data, obs_data_t *settings, bool msg_gui)
{
	switch (obs_data_get_int(settings, "login_type")) {
	case RTMP_NICOLIVE_LOGIN_MAIL:
		nicolive_set_settings(data,
		    obs_data_get_string(settings, "mail"),
		    obs_data_get_string(settings, "password"), "");
		if (!nicolive_check_session(data)) {
			nicolive_msg_warn(msg_gui,
			    obs_module_text("MessageFailedLogin"),
			    "failed login");
		}
		break;
	case RTMP_NICOLIVE_LOGIN_SESSION:
		nicolive_set_settings(
		    data, "", "", obs_data_get_string(settings, "session"));
		if (!nicolive_check_session(data)) {
			nicolive_msg_warn(msg_gui,
			    obs_module_text("MessageFailedLogin"),
			    "failed login");
		}
		break;
	case RTMP_NICOLIVE_LOGIN_VIQO:
		if (nicolive_load_viqo_settings(data)) {
			if (!nicolive_check_session(data)) {
				nicolive_msg_warn(msg_gui,
				    obs_module_text("MessageFailedLogin"),
				    "failed login");
			}
		} else {
			nicolive_msg_warn(msg_gui,
			    obs_module_text("MessageFailedLoadViqoSettings"),
			    "failed load viqo settings");
		}
		break;
	default:
		nicolive_msg_error(msg_gui,
		    obs_module_text("MessageFailedLogin"),
		    "unknown login type");
	}

	nicolive_set_enabled_adjust_bitrate(
	    data, obs_data_get_bool(settings, "adjust_bitrate"));

	if (obs_data_get_bool(settings, "auto_start")) {
		nicolive_start_watching(
		    data, obs_data_get_int(settings, "watch_interval"));
	} else {
		nicolive_stop_watching(data);
	}

	// OPTIMIZE: Default settisgs are not saved, so resetting data
	// Maybe obs-studio 0.8.3 bug
	reset_obs_data(bool, settings, "adjust_bitrate");
	reset_obs_data(bool, settings, "auto_start");
	reset_obs_data(int, settings, "watch_interval");
}

// FIXME: why do not call this func. obs-studio 0.8.3 bug?
static void rtmp_nicolive_update(void *data, obs_data_t *settings)
{
	rtmp_nicolive_update_internal(data, settings, true);
}

static void rtmp_nicolive_update_silent(void *data, obs_data_t *settings)
{
	// FIXME: I want to silent with start obs-studio, but saving to setting
	//        call to create service, so I can not silent here.
	// rtmp_nicolive_update_internal(data, settings, false);
	rtmp_nicolive_update_internal(data, settings, true);
}

static void rtmp_nicolive_destroy(void *data) { nicolive_destroy(data); }

static void *rtmp_nicolive_create(obs_data_t *settings, obs_service_t *service)
{
	void *data = nicolive_create();
	UNUSED_PARAMETER(service);

	rtmp_nicolive_update_silent(data, settings);

	return data;
}

static bool rtmp_nicolive_initialize(void *data, obs_output_t *output)
{
	UNUSED_PARAMETER(output);
	bool success = false;
	bool msg_gui = !nicolive_silent_once(data);

	if (nicolive_check_session(data)) {
		if (nicolive_check_live(data)) {
			success = true;
		} else {
			nicolive_msg_warn(msg_gui,
			    obs_module_text("MessageNoLive"),
			    "cannot start streaming: no live");
			success = false;
		}
	} else {
		nicolive_msg_warn(msg_gui,
		    obs_module_text("MessageFailedLogin"),
		    "cannot start streaming: failed login");
		success = false;
	}

	// force to adjust bitrate before starting
	long long bitrate = nicolive_get_live_bitrate(data);
	if (success && nicolive_enabled_adjust_bitrate(data) && bitrate > 0) {
		// ignore fails
		adjust_bitrate(bitrate,
		    obs_encoder_get_settings(
				   obs_output_get_video_encoder(output)),
		    obs_encoder_get_settings(
				   obs_output_get_audio_encoder(output, 0)));
	}

	return success;
}

static void rtmp_nicolive_activate(void *data, obs_data_t *settings)
{
	UNUSED_PARAMETER(settings);
	nicolive_start_streaming(data);
}

static void rtmp_nicolive_deactivate(void *data)
{
	nicolive_stop_streaming(data);
}

static bool change_login_type(
    obs_properties_t *props, obs_property_t *prop, obs_data_t *settings)
{
	UNUSED_PARAMETER(prop);
	switch (obs_data_get_int(settings, "login_type")) {
	case RTMP_NICOLIVE_LOGIN_MAIL:
		obs_property_set_visible(
		    obs_properties_get(props, "mail"), true);
		obs_property_set_visible(
		    obs_properties_get(props, "password"), true);
		obs_property_set_visible(
		    obs_properties_get(props, "session"), false);
		break;
	case RTMP_NICOLIVE_LOGIN_SESSION:
		obs_property_set_visible(
		    obs_properties_get(props, "mail"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "password"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "session"), true);
		break;
	case RTMP_NICOLIVE_LOGIN_VIQO:
		obs_property_set_visible(
		    obs_properties_get(props, "mail"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "password"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "session"), false);
		break;
	default:
		nicolive_log_error("unknown login type");
		return false;
	}
	return true;
}

static bool auto_start_modified(
    obs_properties_t *props, obs_property_t *prop, obs_data_t *settings)
{
	UNUSED_PARAMETER(prop);
	if (obs_data_get_bool(settings, "auto_start")) {
		obs_property_set_enabled(
		    obs_properties_get(props, "watch_interval"), true);
	} else {
		obs_property_set_enabled(
		    obs_properties_get(props, "watch_interval"), false);
	}
	return true;
}

static obs_properties_t *rtmp_nicolive_properties(void *data)
{
	UNUSED_PARAMETER(data);
	obs_property_t *list;
	obs_property_t *prop;
	obs_properties_t *ppts = obs_properties_create();

	list = obs_properties_add_list(ppts, "login_type",
	    obs_module_text("LoginType"), OBS_COMBO_TYPE_LIST,
	    OBS_COMBO_FORMAT_INT);
	obs_property_list_add_int(list, obs_module_text("LoginMailPassowrd"),
	    RTMP_NICOLIVE_LOGIN_MAIL);
	obs_property_list_add_int(list, obs_module_text("UseCookieUserSession"),
	    RTMP_NICOLIVE_LOGIN_SESSION);
	obs_property_list_add_int(list, obs_module_text("LoadViqoSettings"),
	    RTMP_NICOLIVE_LOGIN_VIQO);
	obs_property_set_modified_callback(list, change_login_type);

	obs_properties_add_text(
	    ppts, "mail", obs_module_text("MailAddress"), OBS_TEXT_DEFAULT);
	obs_properties_add_text(
	    ppts, "password", obs_module_text("Password"), OBS_TEXT_PASSWORD);
	obs_properties_add_text(
	    ppts, "session", obs_module_text("Session"), OBS_TEXT_PASSWORD);

	obs_properties_add_bool(
	    ppts, "adjust_bitrate", obs_module_text("AdjustBitrate"));

	prop = obs_properties_add_bool(
	    ppts, "auto_start", obs_module_text("AutoStart"));
	obs_property_set_modified_callback(prop, auto_start_modified);
	obs_properties_add_int(ppts, "watch_interval",
	    obs_module_text("WatchInterval"), 10, 300, 1);

	return ppts;
}

static void rtmp_nicolive_defaults(obs_data_t *settings)
{
	obs_data_set_default_int(
	    settings, "login_type", RTMP_NICOLIVE_LOGIN_MAIL);
	obs_data_set_default_string(settings, "mail", "");
	obs_data_set_default_string(settings, "password", "");
	obs_data_set_default_string(settings, "session", "");
	obs_data_set_default_bool(settings, "adjust_bitrate", true);
	obs_data_set_default_bool(settings, "auto_start", false);
	obs_data_set_default_int(settings, "watch_interval", 60);
}

static const char *rtmp_nicolive_url(void *data)
{
	return nicolive_get_live_url(data);
}

static const char *rtmp_nicolive_key(void *data)
{
	return nicolive_get_live_key(data);
}

static bool rtmp_nicolive_supports_multitrack(void *data)
{
	UNUSED_PARAMETER(data);
	return false;
}

// call func with to update settings
static void rtmp_nicolive_apply_encoder_settings(void *data,
    obs_data_t *video_encoder_settings, obs_data_t *audio_encoder_settings)
{
	if (!(nicolive_check_session(data) && nicolive_check_live(data))) {
		return;
	}

	long long bitrate = nicolive_get_live_bitrate(data);
	if (nicolive_enabled_adjust_bitrate(data) && bitrate > 0) {
		adjust_bitrate(
		    bitrate, video_encoder_settings, audio_encoder_settings);
	}
}

struct obs_service_info rtmp_nicolive_service = {.id = "rtmp_nicolive",
    .get_name = rtmp_nicolive_getname,
    .create = rtmp_nicolive_create,
    .destroy = rtmp_nicolive_destroy,
    .activate = rtmp_nicolive_activate,
    .deactivate = rtmp_nicolive_deactivate,
    .update = rtmp_nicolive_update,
    .get_defaults = rtmp_nicolive_defaults,
    .get_properties = rtmp_nicolive_properties,
    .initialize = rtmp_nicolive_initialize,
    .get_url = rtmp_nicolive_url,
    .get_key = rtmp_nicolive_key,
    .supports_multitrack = rtmp_nicolive_supports_multitrack,
    .apply_encoder_settings = rtmp_nicolive_apply_encoder_settings};

/* module initialize */

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("rtmp-nicolive", "en-US")
OBS_MODULE_AUTHOR("raccy")

bool obs_module_load(void)
{
	obs_register_service(&rtmp_nicolive_service);
	return true;
}

const char *obs_module_name(void)
{
	return obs_module_text("NiconicoLiveModule");
}

const char *obs_module_description(void)
{
	return obs_module_text("NiconicoLiveDescription");
}
