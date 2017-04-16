#include <string>
#include <obs-module.h>
#include "nicookie.h"
#include "nico-live-api.hpp"
#include "nicolive-errno.h"
#include "nicolive-log.h"

namespace
{
char *acquired_session = nullptr;
inline void nicolive_api_set_session(const char *str)
{
	if (acquired_session != nullptr) {
		bfree(acquired_session);
	}
	acquired_session = bstrdup(str);
}
inline void nicolive_api_set_session(const std::string &str)
{
	nicolive_api_set_session(str.c_str());
}
inline const std::string nicolive_api_get_session_nla(const NicoLiveApi &nla)
{
	return nla.getCookie("user_session");
}
}

extern "C" void nicolive_api_clear_session()
{
	if (acquired_session != nullptr) {
		bfree(acquired_session);
		acquired_session = nullptr;
	}
}

extern "C" const char *nicolive_api_get_session() { return acquired_session; }
extern "C" const char *nicolive_api_get_session_login(
    const char *mail, const char *password)
{
	if (mail == nullptr) {
		nicolive_errno = NICOLIVE_ERROR_NO_MAIL_ADDRESS;
		return nullptr;
	}
	if (mail[0] == '\0') {
		nicolive_errno = NICOLIVE_ERROR_EMPTY_MAIL_ADDRESS;
		return nullptr;
	}
	if (password == nullptr) {
		nicolive_errno = NICOLIVE_ERROR_NO_PASSWORD;
		return nullptr;
	}
	if (password[0] == '\0') {
		nicolive_errno = NICOLIVE_ERROR_EMPTY_PASSWORD;
		return nullptr;
	}

	std::string mail_str(mail);
	std::string password_str(password);
	NicoLiveApi nla;
	if (nla.loginSiteNicolive(mail_str, password_str)) {
		nicolive_errno = NICOLIVE_ERROR_LOGIN_SUCCESSFUL;
		nicolive_api_set_session(nicolive_api_get_session_nla(nla));
	} else {
		nicolive_errno = NICOLIVE_ERROR_LOGIN_FAILURE;
		nicolive_api_clear_session();
	}
	return acquired_session;
}
extern "C" const char *nicolive_api_get_session_app(int app)
{
	nicookie_errno = 0;
	const char *session = nicookie_get_session(app);
	if (session != nullptr) {
		nicolive_errno = NICOLIVE_ERROR_COOKIE_ACQUISITION_SUCCESSFUL;
		nicolive_api_set_session(session);
	} else {
		nicolive_errno = NICOLIVE_ERROR_NICOOKIE | nicookie_errno;
		nicolive_api_clear_session();
	}
	return acquired_session;
}

extern "C" bool nicolive_api_check_session(const char *session)
{
	if (session == nullptr) {
		nicolive_errno = NICOLIVE_ERROR_NO_USER_SESSION;
		return false;
	}
	if (session[0] == '\0') {
		nicolive_errno = NICOLIVE_ERROR_EMPTY_USER_SESSION;
		return false;
	}

	std::string session_str(session);
	NicoLiveApi nla;
	nla.setCookie("user_session", session_str);
	const std::string statusXpath = "/getpublishstatus/@status";
	const std::string errorCodeXpath =
	    "/getpublishstatus/error/code/text()";
	std::unordered_map<std::string, std::vector<std::string>> data;
	data[statusXpath] = std::vector<std::string>();
	data[errorCodeXpath] = std::vector<std::string>();
	bool result = nla.getPublishStatus(&data);

	if (!result) return false;
	if (data[statusXpath].empty()) return false;
	std::string &status = data[statusXpath][0];
	if (status == "ok") return true;
	if (status == "fail" && !data[errorCodeXpath].empty() &&
	    data[errorCodeXpath][0] == "notfound")
		return true;
	return false;
}

extern "C" bool nicolive_api_check_login(const char *mail, const char *password)
{
	const char *session = nicolive_api_get_session_login(mail, password);
	if (session != nullptr) {
		return nicolive_api_check_session(session);
	} else {
		return false;
	}
}

extern "C" bool nicolive_api_check_app(int app)
{
	const char *session = nicolive_api_get_session_app(app);
	if (session != nullptr) {
		return nicolive_api_check_session(session);
	} else {
		return false;
	}
}
