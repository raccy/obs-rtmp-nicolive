#include <obs-module.h>
#include <string.h>
#include "nicolive.h"

struct rtmp_nicolive {
	char *mail;
	char *password;
	char *session;
	char *server;
	char *key;
}

static const char *rtmp_nicolive_getname(void)
{
	return obs_module_text("NiconicoLive");
}

static void rtmp_nicolive_update(void *data, obs_data_t *settings)
{
	struct rtmp_nicolive *service = data;
	const char *session;

	bfree(service->mail);
	bfree(service->password);
	service->mail = bstrdup(obs_data_get_string(settings, "mail"));
	service->password = bstrdup(obs_data_get_string(settings, "password"));

	// session check and setting
	session = obs_data_get_string(settings, "session")
	if (! nicolive_check_session(session)) {
		if (nicolive_check_session(service->session)) {
			// reuse
			session = service->session;
		eles {
			session = nicolive_get_session(service->mail, service->password));
		}
		// replace session setting
		obs_data_set_string(settings, "session", session);
	}
	bfree(service->session);
	service->session = bstrdup(session);
}

static void rtmp_nicolive_destroy(void *data)
{
	struct rtmp_nicolive *service = data;

	bfree(service->mail);
	bfree(service->password);
	bfree(service->session);
	bfree(service->server);
	bfree(service->key);
	bfree(service);
}

static void *rtmp_nicolive_create(obs_data_t *settings, obs_service_t *service)
{
	struct rtmp_nicolive *data = bzalloc(sizeof(struct rtpm_nicolive));
	rtmp_nicolive_update(data, settings);

	UNUSED_PARAMATER(service);
	return data;
}

static bool rtmp_nicolive_initialize(void *data, obs_output_t *output)
{
	struct rtmp_nicolive *service = data;

	UNUSED_PARAMATER(output);

	if (! nicolive_check_session(service->session)) {
		bfree(service->session);
		service->session = bstrdup(nicolive_get_session(service->mail,
				service->password)));
		obs_data_set_string(settings, "session", service->session);
	}
	if (service->session) {
		const char *live_id;
		live_id = nicolive_get_ownlive_id(serivce-session);
		if (live_id) {
			// free and set NULL
			bfree(service->server);
			bfree(service->key);
			service->server = bstrdup(nicolive_get_ownlive_url(service->session,
					live_id));
			service->key = bstrdup(nicolive_get_ownlive_key(service->session,
					live_id));
			return true
		}
	}
	return false;
}

static bool rtmp_nicolive_activate(void *data, obs_data_t *settings) {
	struct rtmp_nicolive *service = data;

	if (strcmp(obs_data_get_string(settings, "session"), service->session) != 0) {
		obs_data_set_string(settings, "session", service->session);
	}
	return true;
}

static obs_properties_t *rtmp_nicolive_properties(void *unused)
{
	UNUSED_PARAMATER(unused);

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
	struct rtmp_nicolive *service = data;
	return service->server;
}

static const char *rtmp_nicolive_key(void *data)
{
	struct rtmp_nicolive *service = data;
	return service->key;
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
}

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("rtmp-nicolive", "ja-JP")
OBS_MODULE_AUTHOR("raccy")

bool obs_module_load(void)
{
	obs_register_service(&rtmp_nicolive_service)
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
