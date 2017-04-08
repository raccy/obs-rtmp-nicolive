#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define NICOOKIE_ERROR_NONE 0x0000
#define NICOOKIE_ERROR_NOT_IMPLEMENT 0x0001
#define NICOOKIE_ERROR_UNKNOWN_APP 0x0002
#define NICOOKIE_ERROR_NOT_SUPPORT_APP 0x0003
#define NICOOKIE_ERROR_NOT_FOUND_DATA 0x0004
#define NICOOKIE_ERROR_NOT_FOUND_DATA_FILE 0x0005
#define NICOOKIE_ERROR_INVALID_DATA_FORMAT 0x0006
#define NICOOKIE_ERROR_FAILED_DECRYT 0x0007
#define NICOOKIE_ERROR_FAILED_OPEN_DATA_FILE 0x0008
#define NICOOKIE_ERROR_FAILED_READ_DATA 0x0009
#define NICOOKIE_ERROR_SQLITE 0x0000A
#define NICOOKIE_ERROR_FAILED_PARSE_PROFILE 0x000B
#define NICOOKIE_ERROR_UNKNOWN 0xFFFF

#define NICOOKIE_APP_NONE 0x0000
#define NICOOKIE_APP_IE 0x0001
#define NICOOKIE_APP_EDGE 0x0002
#define NICOOKIE_APP_SAFARI 0x0011
#define NICOOKIE_APP_FIREFOX 0x0021
#define NICOOKIE_APP_CHROME 0x0022
#define NICOOKIE_APP_OPERA 0x0023
#define NICOOKIE_APP_VIQO 0x0101
#define NICOOKIE_APP_UNKNOWN 0xFFFF

extern int nicookie_errno;
// return array end with NICOOKIE_APP_NONE
const int *nicookie_available_apps(void);
const char *nicookie_app_name(int app);
// if error then return NULL
const char *nicookie_get_session(int app);
const char *nicookie_strerror(int errnum);

#ifdef __cplusplus
}
#endif
