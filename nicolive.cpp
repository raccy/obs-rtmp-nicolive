#include <cstdlib>
#include <cstring>
#include <QtCore>
#include <QtNetwork>
#include <obs.h>
#include "nicolive.h"
#include "nico-live.hpp"

// cannot use anonymouse struct because VS2013 bug
// https://connect.microsoft.com/VisualStudio/feedback/details/808506/nsdmi-silently-ignored-on-nested-anonymous-classes-and-structs
static struct nicolive_buff_s {
	char *mail = nullptr;
	char *password = nullptr;
	char *session = nullptr;
	char *live_id = nullptr;
	char *live_url = nullptr;
	char *live_key = nullptr;
} nicolive_buff;


extern "C" void *nicolive_create(void)
{
	return new NicoLive();
}

extern "C" void nicolive_destroy(void *data)
{
	NicoLive *nicolive = static_cast<NicoLive *>(data);
	delete nicolive;
}

extern "C" void nicolive_set_settings(void *data, const char *mail,
	const char *password,const char *session)
{
	NicoLive *nicolive = static_cast<NicoLive *>(data);
	nicolive->setAccount(mail, password);
	nicolive->setSession(session);
}

extern "C" bool nicolive_load_viqo_settings(void *data)
{
	NicoLive *nicolive = static_cast<NicoLive *>(data);
	return nicolive->loadViqoSettings();
}

extern "C" bool nicolive_check_session(void *data)
{
	NicoLive *nicolive = static_cast<NicoLive *>(data);
	return nicolive->checkSession();
}

extern "C" bool nicolive_check_live(void *data)
{
	NicoLive *nicolive = static_cast<NicoLive *>(data);
	return nicolive->checkLive();
}

extern "C" const char *nicolive_get_mail(const void *data)
{
	const NicoLive *nicolive = static_cast<const NicoLive *>(data);
	bfree(nicolive_buff.mail);
	nicolive_buff.mail = bstrdup(nicolive->getMail().toStdString().c_str());
	return nicolive_buff.mail;
}

extern "C" const char *nicolive_get_password(const void *data)
{
	const NicoLive *nicolive = static_cast<const NicoLive *>(data);
	bfree(nicolive_buff.password);
	nicolive_buff.password = bstrdup(
			nicolive->getPassword().toStdString().c_str());
	return nicolive_buff.password;
}

extern "C" const char *nicolive_get_session(const void *data)
{
	const NicoLive *nicolive = static_cast<const NicoLive *>(data);
	bfree(nicolive_buff.session);
	nicolive_buff.session = bstrdup(
			nicolive->getSession().toStdString().c_str());
	return nicolive_buff.session;
}

extern "C" const char *nicolive_get_live_id(const void *data)
{
	const NicoLive *nicolive = static_cast<const NicoLive *>(data);
	bfree(nicolive_buff.live_id);
	nicolive_buff.live_id = bstrdup(
			nicolive->getLiveId().toStdString().c_str());
	return nicolive_buff.live_id;
}

extern "C" const char *nicolive_get_live_url(const void *data)
{
	const NicoLive *nicolive = static_cast<const NicoLive *>(data);
	bfree(nicolive_buff.live_url);
	nicolive_buff.live_url = bstrdup(
			nicolive->getLiveUrl().toStdString().c_str());
	return nicolive_buff.live_url;
}

extern "C" const char *nicolive_get_live_key(const void *data)
{
	const NicoLive *nicolive = static_cast<const NicoLive *>(data);
	bfree(nicolive_buff.live_key);
	nicolive_buff.live_key = bstrdup(
			nicolive->getLiveKey().toStdString().c_str());
	return nicolive_buff.live_key;
}
