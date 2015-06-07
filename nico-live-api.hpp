#pragma once

#include <string>
#include <map>

class NicoLiveApi {
public:
	static const std::string LOGIN_SITE_URL;
	static const std::string LOGIN_API_URL;
	static const std::string PUBSTAT_URL;
	static std::string createWwwFormUrlencoded(
		const std::multimap<std::string, std::string> &formData);
	static std::string createCookieString(
		const std::map<std::string, std::string> &cookie);
	static bool parseXml(
		const std::string &xml,
		std::multimap<std::string, std::string> *data);
	static std::string urlEncode(const std::string str);

private:
	std::map<std::string, std::string> cookie;

public:
	NicoLiveApi();
	~NicoLiveApi();

	// Cookie
	void setCookie(const std::string &name, const std::string &value);
	void deleteCookie(const std::string &name);
	void clearCookie();
	const std::string getCookie(const std::string &name) const;

	// Generic
	bool getWeb(
		const std::string &url,
		int *code,
		std::string *response);
	bool postWeb(
		const std::string &url,
		const std::multimap<std::string, std::string> &formData,
		int *code,
		std::string *response);

	// Nicovideo
	bool loginSite(
		const std::string site,
		const std::string mail,
		const std::string password);
	std::string loginSiteTicket(
		const std::string site,
		const std::string mail,
		const std::string password);
	bool loginSiteNicolive(
		const std::string mail,
		const std::string password);
	std::string loginApiTicket(
		const std::string site,
		const std::string mail,
		const std::string password);
	std::string loginNicoliveEncoder(
		const std::string mail,
		const std::string password);
	bool getPublishStatus(
		std::multimap<std::string, std::string>);
	bool getPublishStatusTicket(
		const std::string &ticket,
		std::multimap<std::string, std::string> *data);
};
