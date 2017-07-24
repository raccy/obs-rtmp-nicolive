/**
 * Nicookie C++ (better C)
 */
#include "nicookie.h"
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include "json/json.hpp"
// #include "sqlite/sqlite3.h"

// Globale Objects
int nicookie_errno = NICOOKIE_ERROR_NONE;

namespace
{
using json = nlohmann::json;
// Private Globale Objects
const int availableApps[] = {
#ifdef _WIN32
    NICOOKIE_APP_IE, NICOOKIE_APP_EDGE,
#elif __APPLE__
    NICOOKIE_APP_SAFARI,
#endif
    NICOOKIE_APP_FIREFOX, NICOOKIE_APP_CHROME, NICOOKIE_APP_OPERA,
    NICOOKIE_APP_VIQO, NICOOKIE_APP_NONE};
std::string cookieSession = "";

// Directory Paths
#ifdef _WIN32
const char *PATH_SEPARATOR = "\\";
#else
const char *PATH_SEPARATOR = "/";
#endif
inline std::string HomeDirectory()
{
	std::string str = "";
#ifdef _WIN32
	str += getenv("HOMEDRIVE");
	str += getenv("HOMEPATH");
#else
	str += getenv("HOME");
#endif
	return str;
}
#ifdef _WIN32
inline std::string AppDataDirectory() { return getenv("APPDATA"); }
inline std::string LocalAppDataDirectory() { return getenv("LOCALAPPDATA"); }
#elif __APPLE__
inline std::string AppDataDirectory()
{
	std::string str = "";
	str += HomeDirectory();
	str += "/Library/Application Support";
	return str;
}
#else
#endif

std::string FindViqo();

std::string FindViqo()
{
	std::string settingsJson = "";
#ifdef _WIN32
	settingsJson += LocalAppDataDirectory();
#elif __APPLE__
	settingsJson += AppDataDirectory();
#else
	settingsJson += HomeDirectory();
	settingsJson += "/.local/share";
#endif
	settingsJson += PATH_SEPARATOR;
	settingsJson += "Viqo";
	settingsJson += PATH_SEPARATOR;
	settingsJson += "settings.json";

	errno = 0;
	std::ifstream ifs(settingsJson);
	if (ifs.fail()) {
		if (errno == ENOENT) {
			nicookie_errno = NICOOKIE_ERROR_NOT_FOUND_FILE;
		} else {
			nicookie_errno = NICOOKIE_ERROR_FAILED_OPEN_DATA_FILE;
		}
		return "";
	}
	json j;
	ifs >> j;
	std::string session = j["login_way"]["user_session"];
	if (session.empty()) {
		nicookie_errno = NICOOKIE_ERROR_NOT_FOUND_DATA;
		return "";
	}
	return session;
};

// class Nicookie
// {
// 	enum nicookie_error error;
// 	std::string session;
// 	std::vector<enum nicookie_app> appList;
//
// public:
// 	Nicookie();
// 	const std::vector<enum nicookie_app> &GetAppList() const;
// 	const std::string GetSession();
// 	enum nicookie_error GetError() const;
// 	static const std::string &AppName(enum nicookie_app app);
// 	static const std::string &StrError(enum nicookie_error errnum);
// };
//
// Nicookie::Nicookie() : error(NICOOKIE_ERROR_NONE), session(""),
// appList()
// {
// #ifdef _WIN32
// 	this->appList.push_back(NICOOKIE_APP_IE);
// 	this->appList.push_back(NICOOKIE_APP_EDGE);
// #elif __APPLE__
// 	this->appList.push_back(NICOOKIE_APP_SAFARI);
// #endif
// 	this->appList.push_back(NICOOKIE_APP_FIREFOX);
// 	this->appList.push_back(NICOOKIE_APP_CHROME);
// 	this->appList.push_back(NICOOKIE_APP_OPERA);
// 	this->appList.push_back(NICOOKIE_APP_NONE);
// }
// const std::vector<enum nicookie_app> &Nicookie::GetAppList() const
// {
// 	return this->appList;
// }
// const std::string Nicookie::GetSession()
// {
// 	// TODO
// 	return this->session;
// }
// enum nicookie_error Nicookie::GetError() const { return this->error;
// };
// const std::string &Nicookie::AppName(enum nicookie_app app) {}
// const std::string &Nicookie::StrError(enum nicookie_error errnum) {}
// static Nicookie nc;
}

// extern C interface
extern "C" const int *nicookie_available_apps() { return availableApps; }
extern "C" const char *nicookie_app_name(int app)
{
	switch (app) {
	case NICOOKIE_APP_NONE:
		return "";
		break;
	case NICOOKIE_APP_IE:
		return "Internet Explorer";
		break;
	case NICOOKIE_APP_EDGE:
		return "Microsoft Edge";
		break;
	case NICOOKIE_APP_SAFARI:
		return "Safari";
		break;
	case NICOOKIE_APP_FIREFOX:
		return "Mozilla Firefox";
		break;
	case NICOOKIE_APP_CHROME:
		return "Google Chrome";
		break;
	case NICOOKIE_APP_OPERA:
		return "Opera";
		break;
	case NICOOKIE_APP_VIQO:
		return "Viqo";
		break;
	}
	return "Unknown";
}

extern "C" const char *nicookie_get_session(int app)
{
	cookieSession.clear();
	switch (app) {
	case NICOOKIE_APP_NONE:
		nicookie_errno = NICOOKIE_ERROR_UNKNOWN_APP;
		break;
	case NICOOKIE_APP_IE:
		nicookie_errno = NICOOKIE_ERROR_NOT_IMPLEMENT;
		break;
	case NICOOKIE_APP_EDGE:
		nicookie_errno = NICOOKIE_ERROR_NOT_IMPLEMENT;
		break;
	case NICOOKIE_APP_SAFARI:
		nicookie_errno = NICOOKIE_ERROR_NOT_IMPLEMENT;
		break;
	case NICOOKIE_APP_FIREFOX:
		nicookie_errno = NICOOKIE_ERROR_NOT_IMPLEMENT;
		break;
	case NICOOKIE_APP_CHROME:
		nicookie_errno = NICOOKIE_ERROR_NOT_IMPLEMENT;
		break;
	case NICOOKIE_APP_OPERA:
		nicookie_errno = NICOOKIE_ERROR_NOT_IMPLEMENT;
		break;
	case NICOOKIE_APP_VIQO:
		cookieSession = FindViqo();
		break;
	default:
		nicookie_errno = NICOOKIE_ERROR_UNKNOWN_APP;
	}
	if (cookieSession.empty()) {
		return nullptr;
	}
	return cookieSession.c_str();
}
extern "C" const char *nicookie_strerror(int errnum)
{
	switch (errnum) {
	case NICOOKIE_ERROR_NONE:
		return "No error.";
		break;
	case NICOOKIE_ERROR_NOT_IMPLEMENT:
		return "The operation is not implemented.";
		break;
	case NICOOKIE_ERROR_UNKNOWN_APP:
		return "An unknown applaction was specified.";
		break;
	case NICOOKIE_ERROR_NOT_SUPPORT_APP:
		return "The specified application is not supported.";
		break;
	case NICOOKIE_ERROR_NOT_FOUND_DATA:
		return "Data was not found.";
		break;
	case NICOOKIE_ERROR_NOT_FOUND_FILE:
		return "Data file was not found.";
		break;
	case NICOOKIE_ERROR_INVALID_DATA_FORMAT:
		return "The data format is invaild.";
		break;
	case NICOOKIE_ERROR_FAILED_DECRYT:
		return "Decryption failed.";
		break;
	case NICOOKIE_ERROR_FAILED_OPEN_DATA_FILE:
		return "Could not open data file.";
		break;
	case NICOOKIE_ERROR_FAILED_READ_DATA:
		return "Could not read file.";
		break;
	case NICOOKIE_ERROR_SQLITE:
		return "Error was occured in reading SQLite3";
		break;
	case NICOOKIE_ERROR_FAILED_PARSE_PROFILE:
		return "Error was occured in parsing a profile file";
		break;
	}
	return "Unknown error hos occured.";
}
