#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum nicookie_error {
	NICOOKIE_ERROR_NONE = 0,
	NICOOKIE_ERROR_UNKNOWN,
	NICOOKIE_ERROR_NOT_IMPLEMENT,
	NICOOKIE_ERROR_UNKNOWN_APP,
	NICOOKIE_ERROR_NOT_SUPPORT_APP,
	NICOOKIE_ERROR_NOT_FOUND_DATA,
	NICOOKIE_ERROR_NOT_FOUND_DATA_FILE,
	NICOOKIE_ERROR_INVALID_DATA_FORMAT,
	NICOOKIE_ERROR_FAILED_DECRYT,
	NICOOKIE_ERROR_FAILED_OPEN_DATA_FILE,
	NICOOKIE_ERROR_FAILED_READ_DATA,
	NICOOKIE_ERROR_SQLITE,
	NICOOKIE_ERROR_FAILED_PARSE_PROFILE,
};

enum nicookie_app {
	NICOOKIE_APP_NONE = 0,
	NICOOKIE_APP_UNKNOWN,
	NICOOKIE_APP_IE,
	NICOOKIE_APP_EDGE,
	NICOOKIE_APP_SAFARI,
	NICOOKIE_APP_FIREFOX,
	NICOOKIE_APP_CHROME,
	NICOOKIE_APP_OPERA,
	NICOOKIE_APP_VIQO,
};

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
