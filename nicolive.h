#pragma once

#include <stdbool.h>

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

bool nicolive_chek_session_n(const char *session);

static inline bool nicolive_check_session(const char *session) {
	if (!session || !*session) {
		return false;
	}
	return nicolive_chek_session_n(session);
}

void *nicolive_create(void);
void nicolive_destroy(void *data);

void nicolive_set_settings(const char *mail, const char *password,
	const char *session);
bool nicolive_load_viqo_settings(void);

bool nicolive_is_login(void *data);
bool nicolive_is_live(void *data);

const char *nicolive_get_mail(void *data);
const char *nicolive_get_password(void *data);
const char *nicolive_get_session(void *data);
const char *nicolive_get_live_url(void *data);
const char *nicolive_get_live_key(void *data);

#ifdef __cplusplus
}
#endif
