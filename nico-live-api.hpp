#pragma once

#include <string>
#include <map>


class NicoLiveApi {
	typedef std::multimap<std::string, std::string> Cookie;
public:
	NicoLiveApi();
	~NicoLiveApi();

	bool setCookie(
		const std::multimap<std::string, std::string> &cookie);
	const std::multimap<std::string, std::string> &getCookie() const;


	// Generic
	bool getWeb(
		const std::string &url,
		int *code,
		std::string *response,
		std::multimap<std::string, std::string> &cookie);
	bool postWeb(
		const std::string &url,
		const std::multimap<std::string, std::string> &formData,
		int *code,
		std::string *response,
		std::multimap<std::string, std::string> &cookie);
	std::string createWwwFormUrlEncoded(
		const std::multimap<std::string, std::string> &formData);
	std::string createCookieString(
		const std::multimap<std::string, std::string> &cookie);
	// Nicovideo
	bool loginSite(
		const std::string site,
		const std::string mail,
		const std::string password
	);
	std::string loginSiteTicket(
		const std::string site,
		const std::string mail,
		const std::string password
	);
	std::string loginSiteNicolive(
		const std::string mail,
		const std::string password
	);
	std::string loginApiTicket(
		const std::string site,
		const std::string mail,
		const std::string password
	);
	std::string loginNicoliveEncoder(
		const std::string mail,
		const std::string password
	);

};
