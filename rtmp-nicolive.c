#include <stdbool.h>
#include <string.h>
#include <obs-module.h>
#include "nicookie.h"
#include "nicolive-api.h"
#include "nicolive-errno.h"
#include "nicolive-log.h"
#include "nicolive.h"

// use in set_data_nicolive for reset default settigs
// #define reset_obs_data(type, settings, name) \
// 	obs_data_set_##type(                 \
// 	    (settings), (name), obs_data_get_##type((settings), (name)))

// enum rtmp_nicolive_login_type {
// 	RTMP_NICOLIVE_LOGIN_MAIL,
// 	RTMP_NICOLIVE_LOGIN_SESSION,
// 	RTMP_NICOLIVE_LOGIN_APP,
// };

enum rtmp_nicolive_session_method {
	RTMP_NICOLIVE_SESSION_LOGIN,
	RTMP_NICOLIVE_SESSION_APP,
	RTMP_NICOLIVE_SESSION_MANUAL,
};

// use on callback for check button
static obs_data_t *current_settings = NULL;
// uso on callback for modified settings
static obs_data_t *checked_settings = NULL;

/* utilities */

inline static bool same_data_string(
    obs_data_t *a, obs_data_t *b, const char *name)
{
	return strcmp(obs_data_get_string(a, name),
		   obs_data_get_string(b, name)) == 0;
}

inline static bool same_data_int(obs_data_t *a, obs_data_t *b, const char *name)
{
	return obs_data_get_int(a, name) == obs_data_get_int(b, name);
}

inline static bool same_checked_settings(obs_data_t *settings)
{
	if (checked_settings == NULL) return false;
	if (!same_data_int(checked_settings, settings, "session_method"))
		return false;
	switch (obs_data_get_int(settings, "session_method")) {
	case RTMP_NICOLIVE_SESSION_LOGIN:
		return same_data_string(checked_settings, settings, "mail") &&
		       same_data_string(checked_settings, settings, "password");
	case RTMP_NICOLIVE_SESSION_APP:
		return same_data_int(checked_settings, settings, "cookie_app");
	case RTMP_NICOLIVE_SESSION_MANUAL:
		return same_data_string(checked_settings, settings, "session");
	}
	return false;
}

/** get session and check session.
 * If a session is valid, then set session on settings and return true,
 * else clear session on settings and false.
 * @param settings read and write obs settings
 * @return boolean that the session is valid
 */
inline static bool check_settings(obs_data_t *settings)
{
	// first check that the setting has checkd, if ok then return
	// true
	if (obs_data_get_bool(settings, "checked")) {
		// do not change
		return true;
	}

	nicolive_errno = 0;
	bool check_ok = false;
	const char *additional_message = NULL;
	const char *session = obs_data_get_string(settings, "session");
	// second check that the session is valid, if ok then return
	// true
	check_ok = nicolive_api_check_session(session);

	// third get session and check session
	if (!check_ok) {
		switch (obs_data_get_int(settings, "session_method")) {
		case RTMP_NICOLIVE_SESSION_LOGIN:
			session = nicolive_api_get_session_login(
			    obs_data_get_string(settings, "mail"),
			    obs_data_get_string(settings, "password"));

			if (session == NULL) {
				nicolive_log_warn("failed to login");
			} else {
				nicolive_log_info("succeded to login");
				check_ok = nicolive_api_check_session(session);
			}
			break;
		case RTMP_NICOLIVE_SESSION_APP:
			session = nicolive_api_get_session_app(
			    obs_data_get_int(settings, "cookie_app"));
			if (session == NULL) {
				nicolive_log_warn("failed to get session");
			} else {
				nicolive_log_info("succeded to get session");
				check_ok = nicolive_api_check_session(session);
			}
			break;
		case RTMP_NICOLIVE_SESSION_MANUAL:
			// already checkd, and faield, so do nothing
			break;
		default:
			nicolive_log_error("unknown session method");
			additional_message =
			    obs_module_text("UnknownSessionMethod");
			check_ok = false;
		}
	}

	const char *result_message = NULL;
	if (check_ok) {
		nicolive_log_info("valid user session cookie");
		obs_data_set_string(settings, "session", session);
		obs_data_set_bool(settings, "checked", true);
		if (checked_settings == NULL)
			checked_settings = obs_data_create();
		obs_data_apply(checked_settings, settings);
		result_message = obs_module_text("Succeeded");
	} else {
		nicolive_log_warn("invalid user session cookie");
		obs_data_set_bool(settings, "checked", false);
		result_message = obs_module_text("Failed");
	}

	if (nicolive_errno != 0 && additional_message == NULL) {
		additional_message = nicolive_strerror(nicolive_errno);
	}

	char *message = NULL;
	if (additional_message == NULL) {
		message = bstrdup(result_message);
	} else {
		message = bzalloc(
		    strlen(result_message) + strlen(additional_message) + 2);
		strcat(message, result_message);
		strcat(message, ":");
		strcat(message, additional_message);
	}
	obs_data_set_string(settings, "check_message", message);
	bfree(message);

	return check_ok;
}

inline static void set_data_nicolive(void *data, obs_data_t *settings)
{
	if (check_settings(settings)) {
		nicolive_set_settings(
		    data, "", "", obs_data_get_string(settings, "session"));
		// OPTIMIZE: double check?
		// if (!nicolive_check_session(data)) {
		// 	nicolive_log_warn("invalid session");
		// }
	} else {
		// reset session
		nicolive_set_settings(data, "", "", "");
	}

	nicolive_set_enabled_adjust_bitrate(
	    data, obs_data_get_bool(settings, "adjust_bitrate"));

	if (obs_data_get_bool(settings, "auto_start")) {
		nicolive_start_watching(data);
	} else {
		nicolive_stop_watching(data);
	}
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
		nicolive_log_warn("audio bitrate is too high");
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

/* property events */

inline static bool on_modified_settings(
    obs_properties_t *props, obs_property_t *property, obs_data_t *settings)
{
	nicolive_log_debug_call_func();
	if (same_checked_settings(settings)) {
		if (!obs_data_get_bool(settings, "checked")) {
			obs_data_set_bool(settings, "checked", true);
			obs_data_set_string(settings, "check_message",
			    obs_module_text("Checked"));
		}

	} else {
		if (obs_data_get_bool(settings, "checked")) {
			obs_data_set_bool(settings, "checked", false);
			obs_data_set_string(settings, "check_message",
			    obs_module_text("Unchecked"));
		}
	}
	return true;
}

inline static bool on_modified_session_method(
    obs_properties_t *props, obs_property_t *property, obs_data_t *settings)
{
	on_modified_settings(props, property, settings);
	// update current settings
	current_settings = settings;
	switch (obs_data_get_int(settings, "session_method")) {
	case RTMP_NICOLIVE_SESSION_LOGIN:
		obs_property_set_visible(
		    obs_properties_get(props, "mail"), true);
		obs_property_set_visible(
		    obs_properties_get(props, "password"), true);
		obs_property_set_visible(
		    obs_properties_get(props, "session"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "cookie_app"), false);
		break;
	case RTMP_NICOLIVE_SESSION_APP:
		obs_property_set_visible(
		    obs_properties_get(props, "mail"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "password"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "session"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "cookie_app"), true);
		break;
	case RTMP_NICOLIVE_SESSION_MANUAL:
		obs_property_set_visible(
		    obs_properties_get(props, "mail"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "password"), false);
		obs_property_set_visible(
		    obs_properties_get(props, "session"), true);
		obs_property_set_visible(
		    obs_properties_get(props, "cookie_app"), false);
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
	return check_settings(current_settings);
}

/* module operations */

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
	if (check_settings(settings)) {
		set_data_nicolive(data, settings);
	}
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
	if (check_settings(settings)) {
		set_data_nicolive(data, settings);
	}
}

static void rtmp_nicolive_defaults(obs_data_t *settings)
{
	nicolive_log_debug_call_func();
	obs_data_set_default_int(
	    settings, "session_method", RTMP_NICOLIVE_SESSION_LOGIN);
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

	obs_property_t *prop_session_method = obs_properties_add_list(ppts,
	    "session_method", obs_module_text("SessionSettingMethod"),
	    OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
	obs_property_list_add_int(prop_session_method,
	    obs_module_text("LoginMailPassowrd"), RTMP_NICOLIVE_SESSION_LOGIN);
	obs_property_list_add_int(prop_session_method,
	    obs_module_text("LoadAppSettings"), RTMP_NICOLIVE_SESSION_APP);
	obs_property_list_add_int(prop_session_method,
	    obs_module_text("ManualInputUserSession"),
	    RTMP_NICOLIVE_SESSION_MANUAL);
	obs_property_set_modified_callback(
	    prop_session_method, on_modified_session_method);

	// login mail
	obs_property_t *prop_mail = obs_properties_add_text(
	    ppts, "mail", obs_module_text("MailAddress"), OBS_TEXT_DEFAULT);
	obs_property_set_modified_callback(prop_mail, on_modified_settings);
	obs_property_t *prop_password = obs_properties_add_text(
	    ppts, "password", obs_module_text("Password"), OBS_TEXT_PASSWORD);
	obs_property_set_modified_callback(prop_password, on_modified_settings);

	// login session
	obs_property_t *prop_session = obs_properties_add_text(
	    ppts, "session", obs_module_text("Session"), OBS_TEXT_PASSWORD);
	obs_property_set_modified_callback(prop_session, on_modified_settings);

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
	obs_property_set_modified_callback(
	    prop_cookie_app, on_modified_settings);

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
			nicolive_log_warn("cannot start streaming: no live");
			success = false;
		}
	} else {
		nicolive_log_warn("cannot start streaming: failed login");
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

/* module structure */

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
