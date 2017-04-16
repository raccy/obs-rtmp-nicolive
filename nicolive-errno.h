#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum nicolive_error {
	NICOLIVE_ERROR_NONE = 0,
	NICOLIVE_ERROR_ACCESS_ERROR,
	// login with mail address and password
	NICOLIVE_ERROR_LOGIN_SUCCESSFUL = 0x100,
	NICOLIVE_ERROR_LOGIN_FAILURE,
	NICOLIVE_ERROR_NO_MAIL_ADDRESS,
	NICOLIVE_ERROR_EMPTY_MAIL_ADDRESS,
	NICOLIVE_ERROR_ILLEGAL_TYPE_MAIL_ADDRESS,
	NICOLIVE_ERROR_NO_PASSWORD,
	NICOLIVE_ERROR_EMPTY_PASSWORD,
	// check session
	NICOLIVE_ERROR_VALID_USER_SESSION = 0x200,
	NICOLIVE_ERROR_INVALID_USER_SESSION,
	NICOLIVE_ERROR_NO_USER_SESSION,
	NICOLIVE_ERROR_EMPTY_USER_SESSION,
	NICOLIVE_ERROR_ILLEGAL_TYPE_USER_SESSION,
	// cookie acquisition
	NICOLIVE_ERROR_COOKIE_ACQUISITION_SUCCESSFUL = 0x300,
	NICOLIVE_ERROR_COOKIE_ACQUISITION_FAILURE,
	NICOLIVE_ERROR_NICOOKIE = 0x1000,
	NICOLIVE_ERROR_UNKNOWN = 0xFFFF,
};

extern int nicolive_errno;
const char *nicolive_strerror(int errnum);

#ifdef __cplusplus
}
#endif