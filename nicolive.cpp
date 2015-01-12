#include "nicolive.h"

// static char *nikolive_session;

extern "C" bool nicolive_chek_session_n(const char *session)
{
	return true;
}

// get new sesson
extern "C" const char *nicolive_get_session(const char *mail,
	const char *password)
{
	return "dummy";
}

extern "C" const char *nicolive_get_ownlive_id(const char *session)
{
	return "dummy";
}

extern "C" const char *nicolive_get_ownlive_url(const char *session, const char *live_id);
{
	return "dummy";
}

extern "C" const char *nicolive_get_ownlive_key(const char *session, const char *live_id)
{
	return "dummy";
}
