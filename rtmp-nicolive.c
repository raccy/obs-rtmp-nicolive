#include <stdbool.h>
#include <obs-module.h>
#include "nicookie.h"
#include "nicolive-log.h"
#include "nicolive.h"

// use in set_data_nicolive for reset default settigs
#define reset_obs_data(type, settings, name) \
	obs_data_set_##type(                 \
	    (settings), (name), obs_data_get_##type((settings), (name)))

enum rtmp_nicolive_login_type {
	RTMP_NICOLIVE_LOGIN_MAIL,
	RTMP_NICOLIVE_LOGIN_SESSION,
	RTMP_NICOLIVE_LOGIN_APP,
};

// use on callback for check button
static obs_data_t *current_settings;

inline static bool on_modified_login_type(
    obs_properties_t *props, obs_property_t *property, obs_data_t *settings)
{
	UNUSED_PARAMETER(property);
	// update current settings
	current_settings = settings;

	switch (obs_data_get_int(settings, "login_type")) {
	case RTMP_NICOLIVE_LOGIN_MAIL:
		obs_property_set_visible(
		    obs_properties_get(props, "mail"), true);
		obs_property_set_visible(
		    obs_properties_get(props, "password"), true);
		obs_property_set_visible(
		    obs_properties_get(props, "session"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "cookie_app"), false);
		break;
	case RTMP_NICOLIVE_LOGIN_SESSION:
		obs_property_set_visible(
		    obs_properties_get(props, "mail"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "password"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "session"), true);
		obs_property_set_visible(
		    obs_properties_get(props, "cookie_app"), false);
		break;
	case RTMP_NICOLIVE_LOGIN_APP:
		obs_property_set_visible(
		    obs_properties_get(props, "mail"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "password"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "session"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "cookie_app"), true);
		break;
	default:
		nicolive_log_error("unknown login type");
		return false;
	}
	return true;
}

inline static bool on_clicked_check(
    obs_properties_t *props, obs_property_t *property, void *data)
{
	UNUSED_PARAMETER(props);
	UNUSED_PARAMETER(property);
	UNUSED_PARAMETER(data);
	bool check_ok = false;
	const char *session = NULL;
	switch (obs_data_get_int(current_settings, "login_type")) {
	case RTMP_NICOLIVE_LOGIN_MAIL:
		check_ok = nicolive_test_login(
		    obs_data_get_string(current_settings, "mail"),
		    obs_data_get_string(current_settings, "password"));
		break;
	case RTMP_NICOLIVE_LOGIN_SESSION:
		check_ok = nicolive_test_session(
		    obs_data_get_string(current_settings, "session"));
		break;
	case RTMP_NICOLIVE_LOGIN_APP:
		session = nicookie_get_session(
		    obs_data_get_int(current_settings, "cookie_app"));
		if (session == NULL) {
			nicolive_log_error(
			    "failed load cookie session from app");
			check_ok = false;
			break;
		}
		check_ok = nicolive_test_session(session);
		break;
	default:
		nicolive_log_error("unknown login type");
		obs_data_set_string(current_settings, "check_message",
		    obs_module_text("Failed"));
	}
	if (check_ok) {
		obs_data_set_string(current_settings, "check_message",
		    obs_module_text("Suceeded"));
	} else {
		obs_data_set_string(current_settings, "check_message",
		    obs_module_text("Failed"));
	}
	return true;
}

inline static void set_data_nicolive(
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
	case RTMP_NICOLIVE_LOGIN_APP:
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
		nicolive_start_watching(data);
	} else {
		nicolive_stop_watching(data);
	}

	// OPTIMIZE: Default settisgs are not saved, so resetting data
	// Maybe obs-studio 0.8.3 bug
	reset_obs_data(bool, settings, "adjust_bitrate");
	reset_obs_data(bool, settings, "auto_start");
}

inline static bool adjust_bitrate(long long bitrate,
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

static const char *rtmp_nicolive_getname(void *type_data)
{
	nicolive_log_debug_call_func();
	UNUSED_PARAMETER(type_data);
	return obs_module_text("NiconicoLive");
}

static void *rtmp_nicolive_create(obs_data_t *settings, obs_service_t *service)
{
	nicolive_log_debug_call_func();
	UNUSED_PARAMETER(service);
	void *data = nicolive_create();

	// FIXME: I want to silent with start obs-studio, but saving to setting
	//        call to create service, so I can not silent here.
	// set_data_nicolive(data, settings, false);
	set_data_nicolive(data, settings, true);

	return data;
}

static void rtmp_nicolive_destroy(void *data)
{
	nicolive_log_debug_call_func();
	nicolive_destroy(data);
}

static void rtmp_nicolive_activate(void *data, obs_data_t *settings)
{
	nicolive_log_debug_call_func();
	UNUSED_PARAMETER(settings);
	nicolive_start_streaming(data);
}

static void rtmp_nicolive_deactivate(void *data)
{
	nicolive_log_debug_call_func();
	nicolive_stop_streaming(data);
}

// FIXME: why do not call this func. obs-studio 0.8.3 bug?
static void rtmp_nicolive_update(void *data, obs_data_t *settings)
{
	nicolive_log_debug_call_func();
	set_data_nicolive(data, settings, true);
}

static void rtmp_nicolive_defaults(obs_data_t *settings)
{
	nicolive_log_debug_call_func();
	obs_data_set_default_int(
	    settings, "login_type", RTMP_NICOLIVE_LOGIN_MAIL);
	obs_data_set_default_string(settings, "mail", "");
	obs_data_set_default_string(settings, "password", "");
	obs_data_set_default_string(settings, "session", "");
	obs_data_set_default_int(settings, "cookie_app", NICOOKIE_APP_NONE);
	obs_data_set_default_string(
	    settings, "check_message", obs_module_text("Unchecked"));
	obs_data_set_default_bool(settings, "adjust_bitrate", true);
	obs_data_set_default_bool(settings, "auto_start", false);
}

static obs_properties_t *rtmp_nicolive_properties(void *data)
{
	nicolive_log_debug_call_func();
	UNUSED_PARAMETER(data);
	obs_properties_t *ppts = obs_properties_create();

	obs_property_t *prop_login_type = obs_properties_add_list(ppts,
	    "login_type", obs_module_text("LoginType"), OBS_COMBO_TYPE_LIST,
	    OBS_COMBO_FORMAT_INT);
	obs_property_list_add_int(prop_login_type,
	    obs_module_text("LoginMailPassowrd"), RTMP_NICOLIVE_LOGIN_MAIL);
	obs_property_list_add_int(prop_login_type,
	    obs_module_text("UseCookieUserSession"),
	    RTMP_NICOLIVE_LOGIN_SESSION);
	obs_property_list_add_int(prop_login_type,
	    obs_module_text("LoadAppSettings"), RTMP_NICOLIVE_LOGIN_APP);
	obs_property_set_modified_callback(
	    prop_login_type, on_modified_login_type);

	// login mail
	obs_properties_add_text(
	    ppts, "mail", obs_module_text("MailAddress"), OBS_TEXT_DEFAULT);
	obs_properties_add_text(
	    ppts, "password", obs_module_text("Password"), OBS_TEXT_PASSWORD);

	// login session
	obs_properties_add_text(
	    ppts, "session", obs_module_text("Session"), OBS_TEXT_PASSWORD);

	// login app
	obs_property_t *prop_cookie_app = obs_properties_add_list(ppts,
	    "cookie_app", obs_module_text("LoadCookieApp"), OBS_COMBO_TYPE_LIST,
	    OBS_COMBO_FORMAT_INT);
	const int *available_apps = nicookie_available_apps();
	for (const int *app_p = available_apps; *app_p != NICOOKIE_APP_NONE;
	     app_p++) {
		nicolive_log_debug("add list app: %d", *app_p);
		obs_property_list_add_int(
		    prop_cookie_app, nicookie_app_name(*app_p), *app_p);
	}

	obs_properties_add_button(
	    ppts, "check", obs_module_text("Check"), on_clicked_check);
	obs_property_t *prop_check_message = obs_properties_add_text(
	    ppts, "check_message", "", OBS_TEXT_DEFAULT);
	obs_property_set_enabled(prop_check_message, false);

	obs_properties_add_bool(
	    ppts, "adjust_bitrate", obs_module_text("AdjustBitrate"));

	obs_properties_add_bool(
	    ppts, "auto_start", obs_module_text("AutoStart"));

	return ppts;
}

static bool rtmp_nicolive_initialize(void *data, obs_output_t *output)
{
	nicolive_log_debug_call_func();
	UNUSED_PARAMETER(output);
	bool success = false;

	if (nicolive_check_session(data)) {
		if (nicolive_check_live(data)) {
			success = true;
		} else {
			nicolive_msg_warn(false,
			    obs_module_text("MessageNoLive"),
			    "cannot start streaming: no live");
			success = false;
		}
	} else {
		nicolive_msg_warn(false, obs_module_text("MessageFailedLogin"),
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

static const char *rtmp_nicolive_url(void *data)
{
	nicolive_log_debug_call_func();
	return nicolive_get_live_url(data);
}

static const char *rtmp_nicolive_key(void *data)
{
	nicolive_log_debug_call_func();
	return nicolive_get_live_key(data);
}

static bool rtmp_nicolive_supports_multitrack(void *data)
{
	nicolive_log_debug_call_func();
	UNUSED_PARAMETER(data);
	return false;
}

// call func with to update settings
static void rtmp_nicolive_apply_encoder_settings(void *data,
    obs_data_t *video_encoder_settings, obs_data_t *audio_encoder_settings)
{
	nicolive_log_debug_call_func();
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
