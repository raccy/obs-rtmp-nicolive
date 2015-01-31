#include <string.h>
#include <stdbool.h>
#include <obs-module.h>
#include "nicolive.h"
#include "nicolive-ui.h"

static const char *rtmp_nicolive_getname(void)
{
	return obs_module_text("NiconicoLive");
}

static void rtmp_nicolive_update(void *data, obs_data_t *settings)
{
	nicolive_set_settings(data,
			obs_data_get_string(settings, "mail"),
			obs_data_get_string(settings, "password"),
			obs_data_get_string(settings, "session"));
}

static void rtmp_nicolive_destroy(void *data)
{
	nicolive_destroy(data);
}

static void *rtmp_nicolive_create(obs_data_t *settings, obs_service_t *service)
{
	void *data = nicolive_create();
	UNUSED_PARAMETER(service);

	rtmp_nicolive_update(data, settings);

	return data;
}

static bool rtmp_nicolive_initialize(void *data, obs_output_t *output)
{
	UNUSED_PARAMETER(output);

	if (nicolive_check_session(data))
		if (nicolive_check_live(data))
			return true;
		else
			nicolive_mbox_warn(obs_module_text("MessageNoLive"));
	else
		nicolive_mbox_warn(obs_module_text("MessageLoginFailed"));
	return false;
}

static void rtmp_nicolive_activate(void *data, obs_data_t *settings) {
	obs_data_set_string(settings, "session", nicolive_get_session(data));
}

static obs_properties_t *rtmp_nicolive_properties(void *unused)
{
	UNUSED_PARAMETER(unused);

	obs_properties_t *ppts = obs_properties_create();
	obs_properties_add_text(ppts, "mail", obs_module_text("MailAddress"),
			OBS_TEXT_DEFAULT);
	obs_properties_add_text(ppts, "password", obs_module_text("Password"),
			OBS_TEXT_PASSWORD);
	obs_properties_add_text(ppts, "session", obs_module_text("Session"),
			OBS_TEXT_PASSWORD);

	return ppts;
}

static const char *rtmp_nicolive_url(void *data)
{
	return nicolive_get_live_url(data);
}

static const char *rtmp_nicolive_key(void *data)
{
	return nicolive_get_live_key(data);
}

struct obs_service_info rtmp_nicolive_service = {
	.id             = "rtmp_nicolive",
	.get_name       = rtmp_nicolive_getname,
	.create         = rtmp_nicolive_create,
	.destroy        = rtmp_nicolive_destroy,
	.update         = rtmp_nicolive_update,
	.initialize     = rtmp_nicolive_initialize,
	.activate       = rtmp_nicolive_activate,
	.get_properties = rtmp_nicolive_properties,
	.get_url        = rtmp_nicolive_url,
	.get_key        = rtmp_nicolive_key
};

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
