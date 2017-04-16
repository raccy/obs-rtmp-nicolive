#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

void nicolive_api_clear_session(void);

const char *nicolive_api_get_session(void);
const char *nicolive_api_get_session_login(
    const char *mail, const char *password);
const char *nicolive_api_get_session_app(int app);

bool nicolive_api_check_session(const char *session);
bool nicolive_api_check_login(const char *mail, const char *password);
bool nicolive_api_check_app(int app);

#ifdef __cplusplus
}
#endif
