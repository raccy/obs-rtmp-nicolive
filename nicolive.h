#pragma once

#ifndef __func__
#define __func__ __FUNCTION__
#endif
#define error(format, ...) \
		blog(LOG_ERROR, "[nicolive] " format, ##__VA_ARGS__)
#define warn(format, ...) \
		blog(LOG_WARNING, "[nicolive] " format, ##__VA_ARGS__)
#define info(format, ...) \
		blog(LOG_INFO, "[nicolive] " format, ##__VA_ARGS__)

#ifdef _DEBUG
#define debug(format, ...) \
		blog(LOG_DEBUG, "[nicolive] " format, ##__VA_ARGS__)
#define debug_call_func() \
		debug("call func: %s", __func__)
#else
#define debug(format, ...)
#define debug_call_func()
#endif

#ifdef __cplusplus
extern "C" {
#endif

// check that a session is active without check null or empty
bool nicolive_chek_session_n(const char *session);

// check that a session is active
static inline bool nicolive_check_session(const char *session) {
	if (!session || !*session) {
		return false;
	}
	return nicolive_chek_session_n(session);
}

// get new sesson
const char *nicolive_get_session(const char *mail, const char *password);

// get my live id
const char *nicolive_get_live_id(const char *session);

// get my live url
const char *nicolive_get_live_url(const char *session, const char *live_id);

// get my live key
const char *nicolive_get_live_key(const char *session, const char *live_id);

#ifdef __cplusplus
}
#endif
