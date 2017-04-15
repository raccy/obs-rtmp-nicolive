#include <nicolive-errno.h>
#include <nicookie.h>
#include <obs-module.h>

int nicolive_errno = NICOLIVE_ERROR_NONE;
const char *nicolive_strerror(int errnum)
{
	switch (errnum) {
	case NICOLIVE_ERROR_NONE:
		return obs_module_text("NoError");
	case NICOLIVE_ERROR_ACCESS_ERROR:
		return obs_module_text("AccessError");
	// login with mail address and password
	case NICOLIVE_ERROR_LOGIN_SUCCESSFUL:
		return obs_module_text("LoginSuccessful");
	case NICOLIVE_ERROR_LOGIN_FAILURE:
		return obs_module_text("LoginFailure");
	case NICOLIVE_ERROR_NO_MAIL_ADDRESS:
		return obs_module_text("NoMailAddress");
	case NICOLIVE_ERROR_EMPTY_MAIL_ADDRESS:
		return obs_module_text("EmptyMailAddress");
	case NICOLIVE_ERROR_ILLEGAL_TYPE_MAIL_ADDRESS:
		return obs_module_text("IllegalTypeMailAddress");
	case NICOLIVE_ERROR_NO_PASSWORD:
		return obs_module_text("NoPassword");
	case NICOLIVE_ERROR_EMPTY_PASSWORD:
		return obs_module_text("EmptyPassword");
	// check session
	case NICOLIVE_ERROR_VALID_USER_SESSION:
		return obs_module_text("ValidUserSession");
	case NICOLIVE_ERROR_INVALID_USER_SESSION:
		return obs_module_text("InvalidUserSession");
	case NICOLIVE_ERROR_NO_USER_SESSION:
		return obs_module_text("NoUserSession");
	case NICOLIVE_ERROR_EMPTY_USER_SESSION:
		return obs_module_text("EMPTYUserSession");
	case NICOLIVE_ERROR_ILLEGAL_TYPE_USER_SESSION:
		return obs_module_text("IllegalTypeUserSession");
	// cookie acquisition
	case NICOLIVE_ERROR_COOKIE_ACQUISITION_SUCCESSFUL:
		return obs_module_text("CookieAcquisitionSuccessful");
	case NICOLIVE_ERROR_COOKIE_ACQUISITION_FAILURE:
		return obs_module_text("CookieAcquisitionFailure");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_NONE:
		return obs_module_text("NoError");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_NOT_IMPLEMENT:
		return obs_module_text("UnimplementedApplication");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_UNKNOWN_APP:
		return obs_module_text("UnknownApplication");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_NOT_SUPPORT_APP:
		return obs_module_text("UnsupportedApplication");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_NOT_FOUND_DATA:
		return obs_module_text("DataNotFound");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_NOT_FOUND_FILE:
		return obs_module_text("FileNotFound");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_INVALID_DATA_FORMAT:
		return obs_module_text("BrokenData");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_FAILED_DECRYT:
		return obs_module_text("DecryptionFailure");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_FAILED_OPEN_DATA_FILE:
		return obs_module_text("FailedOpenFile");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_FAILED_READ_DATA:
		return obs_module_text("FailedReadFile");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_SQLITE:
		return obs_module_text("SQLiteError");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_FAILED_PARSE_PROFILE:
		return obs_module_text("FailedParseProfile");
	case NICOLIVE_ERROR_NICOOKIE | NICOOKIE_ERROR_UNKNOWN:
	case NICOLIVE_ERROR_UNKNOWN:
	default:
		return obs_module_text("UnknownError");
	}
}

#ifdef __cplusplus
}
#endif
