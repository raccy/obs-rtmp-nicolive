#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <ios>
#include "nico-live-api.hpp"
#include "pugixml/src/pugixml.hpp"

// static
const std::string NicoLiveApi::LOGIN_SITE_URL =
	"https://secure.nicovideo.jp/secure/login";
const std::string NicoLiveApi::LOGIN_API_URL =
	"https://account.nicovideo.jp/api/v1/login";
const std::string NicoLiveApi::PUBSTAT_URL =
	"http://live.nicovideo.jp/api/getpublishstatus";

std::string NicoLiveApi::createWwwFormUrlencoded(
	const std::unordered_map<std::string, std::string> &formData)
{
	std::string encodedData;
	for (auto &data: formData) {
		if (!encodedData.empty()) {
			encodedData += "&";
		}
		encodedData += NicoLiveApi::urlEncode(data.first);
		encodedData += "=";
		encodedData += NicoLiveApi::urlEncode(data.second);
	}
	return encodedData;
}

std::string NicoLiveApi::createCookieString(
	const std::unordered_map<std::string, std::string> &cookie)
{
	std::string cookieStr;
	for (auto &data: cookie) {
		if (!cookieStr.empty()) {
			cookieStr += "; ";
		}
		cookieStr += data.first;
		cookieStr += "=";
		cookieStr += data.second;
	}
	return cookieStr;
}

bool NicoLiveApi::parseXml(
	const std::string &xml,
	std::unordered_map<std::string, std::vector<std::string>> *data)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_string(xml.c_str());
	if (result.status != pugi::status_ok) {
		return false;
	}
	for (auto &entry: *data) {
		pugi::xpath_node_set nodes =
			doc.select_nodes(entry.first.c_str());
		for (auto &node: nodes) {
			if (node.node() != nullptr) {
				entry.second.push_back(std::string(
					node.node().text().get()));
			} else {
				entry.second.push_back(std::string(
					node.attribute().value()));
			}
		}
	}
	return true;
}

std::string urlEncode(const std::string str)
{
	std::stringstream stream;
	for (const char &ch: str) {
		if (0x20 <= ch && ch <= 0x7F) {
			switch (ch) {
			case '&':
			case '=':
			case '+':
			case '%':
				stream << '%';
				stream << std::setfill ('0')
					<< std::setw(2)
					<< std::hex
					<< std::uppercase
					<< (int)ch;
				break;
			case ' ':
				stream << '+';
				break;
			default:
				stream << ch;
			}
		} else {
			stream << '%';
			stream << std::setfill ('0')
				<< std::setw(2)
				<< std::hex
				<< std::uppercase
				<< (int)ch;
			break;
		}
	}
	return stream.str();
}

// instance
NicoLiveApi::NicoLiveApi()
{}
NicoLiveApi::~NicoLiveApi()
{}
void NicoLiveApi::setCookie(const std::string &name, const std::string &value)
{}
void NicoLiveApi::deleteCookie(const std::string &name)
{}
void NicoLiveApi::clearCookie()
{}
const std::string NicoLiveApi::getCookie(const std::string &name) const
{}
bool NicoLiveApi::getWeb(
	const std::string &url,
	int *code,
	std::string *response)
{}
bool NicoLiveApi::postWeb(
	const std::string &url,
	const std::unordered_map<std::string, std::string> &formData,
	int *code,
	std::string *response)
{}
bool NicoLiveApi::loginSite(
	const std::string site,
	const std::string mail,
	const std::string password)
{}
std::string NicoLiveApi::loginSiteTicket(
	const std::string site,
	const std::string mail,
	const std::string password)
{}
bool NicoLiveApi::loginSiteNicolive(
	const std::string mail,
	const std::string password)
{}
std::string NicoLiveApi::loginApiTicket(
	const std::string site,
	const std::string mail,
	const std::string password)
{}
std::string NicoLiveApi::loginNicoliveEncoder(
	const std::string mail,
	const std::string password)
{}
bool NicoLiveApi::getPublishStatus(
	std::unordered_map<std::string, std::string>)
{}
bool NicoLiveApi::getPublishStatusTicket(
	const std::string &ticket,
	std::unordered_map<std::string, std::string> *data)
{}
