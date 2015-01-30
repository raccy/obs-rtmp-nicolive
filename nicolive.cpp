#include <cstdlib>
#include <cstring>
#include <QtCore>
#include <QtNetwork>
#include <obs.h>
#include "nicolive.h"
#include "nico-live.hpp"

static NicoLive nicolive;

extern "C" bool nicolive_chek_session_n(const char *session)
{
	nicolive.setSession(session);
	return nicolive.checkSession();
}

extern "C" const char *nicolive_get_session(const char *mail,
	const char *password)
{
	nicolive.setAccount(mail, password);
	if (nicolive.siteLogin())
		return nicolive.getSession();
	else
		return NULL;
}

extern "C" const char *nicolive_get_live_id(const char *session)
{
	nicolive.setSession(session);
	return nicolive.getLiveId();
}

extern "C" const char *nicolive_get_live_url(const char *session,
		const char *live_id)
{
	nicolive.setSession(session);
	return nicolive.getLiveUrl(live_id);
}

extern "C" const char *nicolive_get_live_key(const char *session,
		const char *live_id)
{
	nicolive.setSession(session);
	return nicolive.getLiveKey(live_id);
}
