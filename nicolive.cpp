#include "nicolive.h"

#define UNUSED_PARAMETER(param) (void)param

// static char *nikolive_session;

extern "C" bool nicolive_chek_session_n(const char *session)
{
	UNUSED_PARAMETER(session);
	return true;
}

// get new sesson
extern "C" const char *nicolive_get_session(const char *mail,
	const char *password)
{
	UNUSED_PARAMETER(mail);
	UNUSED_PARAMETER(password);
	return "dummy";
}

extern "C" const char *nicolive_get_live_id(const char *session)
{
	UNUSED_PARAMETER(session);
	return "dummy";
}

extern "C" const char *nicolive_get_live_url(const char *session,
		const char *live_id)
{
	UNUSED_PARAMETER(session);
	UNUSED_PARAMETER(live_id);
	return "dummy";
}

extern "C" const char *nicolive_get_live_key(const char *session,
		const char *live_id)
{
	UNUSED_PARAMETER(session);
	UNUSED_PARAMETER(live_id);
	return "dummy";
}
