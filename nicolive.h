#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void *nicolive_create(void);
void nicolive_destroy(void *data);

void nicolive_set_settings(void *data, const char *mail, const char *password,
		const char *session);
void nicolive_set_enabled_adjust_bitrate(void *data, bool enabled);

const char *nicolive_get_mail(const void *data);
const char *nicolive_get_password(const void *data);
const char *nicolive_get_session(const void *data);
const char *nicolive_get_live_url(const void *data);
const char *nicolive_get_live_key(const void *data);
long long nicolive_get_live_bitrate(const void *data);

bool nicolive_enabled_adjust_bitrate(const void *data);

bool nicolive_load_viqo_settings(void *data);
bool nicolive_check_session(void *data);
bool nicolive_check_live(void *data);

void nicolive_start_streaming(void *data);
void nicolive_stop_streaming(void *data);

void nicolive_start_watching(void *data, long long sec);
void nicolive_stop_watching(void *data);

bool nicolive_silent_once(void *data);

#ifdef __cplusplus
}
#endif
