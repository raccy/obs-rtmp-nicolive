#include <obs-module.h>

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
	// TODO: 実装
}

static void rtmp_nicolive_destroy(void *data)
{
	// TODO: 実装
}

static void *rtmp_nicolive_create(obs_data_t *settings, obs_service_t *service)
{
	// TODO: 実装
}

static bool rtmp_nicolive_initialize(void *data, obs_output_t *output)
{
	// TODO: 実装
}

static obs_properties_t *rtmp_nicolive_properties(void *unused)
{
	UNUSED_PARAMATER(unused);

	obs_properties_t *ppts = obs_properties_create();

	obs_properties_add_text(ppts, "mail", obs_module_text("MailAddress"),
			OBS_TEXT_DEFAULT);

	obs_properties_add_text(ppts, "password", obs_module_text("Password"),
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
	.get_properties = rtmp_nicolive_properties,
	.get_url        = rtmp_nicolive_url,
	.get_key        = rtmp_nicolive_key
}

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("rtmp-nicolive", "ja-JP")

bool obs_module_load(void)
{
	obs_register_service(&rtmp_nicolive_service)
	return true;
}
