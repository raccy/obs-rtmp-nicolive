#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class NicoLiveApi
{
	enum class Method {
		GET,
		POST,
	};

public:
	static const std::string LOGIN_SITE_URL;
	static const std::string LOGIN_API_URL;
	static const std::string PUBSTAT_URL;
	static std::string createWwwFormUrlencoded(
			const std::unordered_map<std::string, std::string>
					&formData);
	static std::string createCookieString(
			const std::unordered_map<std::string, std::string>
					&cookie);
	static bool parseXml(const std::string &xml,
			std::unordered_map<std::string,
					     std::vector<std::string>> *data);
	static std::string urlEncode(const std::string &str);
	static size_t writeString(
			char *ptr, size_t size, size_t nmemb, void *userdata);

private:
	std::unordered_map<std::string, std::string> cookie;

public:
	NicoLiveApi();
	~NicoLiveApi();

	// Cookie
	void setCookie(const std::string &name, const std::string &value);
	void deleteCookie(const std::string &name);
	void clearCookie();
	const std::string getCookie(const std::string &name) const;

	// Generic
	bool accessWeb(const std::string &url, const Method &method,
			const std::unordered_map<std::string, std::string>
					&formData,
			int *code, std::string *response);
	bool getWeb(const std::string &url, int *code, std::string *response);
	bool postWeb(const std::string &url,
			const std::unordered_map<std::string, std::string>
					&formData,
			int *code, std::string *response);

	// Nicovideo
	bool loginSite(const std::string &site, const std::string &mail,
			const std::string &password);
	// std::string loginSiteTicket(
	// 	const std::string &site,
	// 	const std::string &mail,
	// 	const std::string &password);
	bool loginSiteNicolive(
			const std::string &mail, const std::string &password);
	std::string loginApiTicket(const std::string &site,
			const std::string &mail, const std::string &password);
	std::string loginNicoliveEncoder(
			const std::string &mail, const std::string &password);
	bool getPublishStatus(std::unordered_map<std::string,
			std::vector<std::string>> *data);
	bool getPublishStatusTicket(
			const std::string &ticket,
			std::unordered_map<std::string,
					std::vector<std::string>> *data);
};
