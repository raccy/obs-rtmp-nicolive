#include "nico-live.hpp"
#include <chrono>
#include <ctime>
#include <curl/curl.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "nicookie.h"
#include "nico-live-api.hpp"
#include "nico-live-watcher.hpp"
#include "nicolive-log.h"
#include "nicolive.h"

namespace
{
// umsvs std::unordered_map<std::string, std::vector<std::string>
// umss std::unordered_map<std::string, std::string>
inline const std::string &get_umsvs_str(
    const std::unordered_map<std::string, std::vector<std::string>> &data,
    const std::unordered_map<std::string, std::string> &map,
    const std::string &name, int pos = 0)
{
	return data.at(map.at(name)).at(pos);
}
inline int get_umsvs_int(
    const std::unordered_map<std::string, std::vector<std::string>> &data,
    const std::unordered_map<std::string, std::string> &map,
    const std::string &name, int pos = 0)
{
	return std::stoi(get_umsvs_str(data, map, name, pos));
}
inline bool get_umsvs_bool(
    const std::unordered_map<std::string, std::vector<std::string>> &data,
    const std::unordered_map<std::string, std::string> &map,
    const std::string &name, int pos = 0)
{
	return !!get_umsvs_int(data, map, name, pos);
}
inline std::chrono::system_clock::time_point get_umsvs_time(
    const std::unordered_map<std::string, std::vector<std::string>> &data,
    const std::unordered_map<std::string, std::string> &map,
    const std::string &name, int pos = 0)
{
	return std::chrono::system_clock::from_time_t(
	    get_umsvs_int(data, map, name, pos));
}
/* const char *name */
inline const std::string &get_umsvs_str(
    const std::unordered_map<std::string, std::vector<std::string>> &data,
    const std::unordered_map<std::string, std::string> &map, const char *name,
    int pos = 0)
{
	return get_umsvs_str(data, map, std::string(name), pos);
}
inline int get_umsvs_int(
    const std::unordered_map<std::string, std::vector<std::string>> &data,
    const std::unordered_map<std::string, std::string> &map, const char *name,
    int pos = 0)
{
	return get_umsvs_int(data, map, std::string(name), pos);
}
inline bool get_umsvs_bool(
    const std::unordered_map<std::string, std::vector<std::string>> &data,
    const std::unordered_map<std::string, std::string> &map, const char *name,
    int pos = 0)
{
	return get_umsvs_bool(data, map, std::string(name), pos);
}
inline std::chrono::system_clock::time_point get_umsvs_time(
    const std::unordered_map<std::string, std::vector<std::string>> &data,
    const std::unordered_map<std::string, std::string> &map, const char *name,
    int pos = 0)
{
	return get_umsvs_time(data, map, std::string(name), pos);
}
}

NicoLive::NicoLive()
{
	watcher = std::unique_ptr<NicoLiveWatcher>(new NicoLiveWatcher(this));
	webApi = std::unique_ptr<NicoLiveApi>(new NicoLiveApi());
}

NicoLive::~NicoLive() {}

void NicoLive::setSession(const std::string &session)
{
	this->session = session;
	this->flags.session_valid = false;
	this->flags.load_viqo = false;
	this->webApi->setCookie("user_session", this->session);
}

void NicoLive::setSession(const char *session)
{
	this->setSession(std::string(session));
}

void NicoLive::setAccount(const std::string &mail, const std::string &password)
{
	this->mail = mail;
	this->password = password;
	this->flags.session_valid = false;
	this->flags.load_viqo = false;
}

void NicoLive::setAccount(const char *mail, const char *password)
{
	this->setAccount(std::string(mail), std::string(password));
}

void NicoLive::setEnabledAdjustBitrate(bool enabled)
{
	this->flags.adjust_bitrate = enabled;
}

const std::string &NicoLive::getMail() const { return this->mail; }

const std::string &NicoLive::getPassword() const { return this->password; }

const std::string &NicoLive::getSession() const { return this->session; }

const std::string &NicoLive::getLiveId() const { return this->live_info.id; }

const std::string &NicoLive::getLiveUrl() const { return this->live_url; }

const std::string &NicoLive::getLiveKey() const { return this->live_key; }

long long NicoLive::getLiveBitrate() const { return this->live_info.bitrate; }

const std::string &NicoLive::getOnairLiveId() const
{
	return this->onair_live_id;
}

bool NicoLive::enabledLive() const
{
	auto now = std::chrono::system_clock::now();
	return this->live_info.start_time <= now &&
	       this->live_info.end_time >= now;
}

NicoLive::LiveState NicoLive::getLiveState() const
{
	auto now = std::chrono::system_clock::now();
	if (now < getLiveStartTime()) {
		return NicoLive::LiveState::BEFORE_START;
	} else if (now <= getLiveEndTime()) {
		return NicoLive::LiveState::ENABLE_LIVE;
	} else {
		return NicoLive::LiveState::AFTER_END;
	}
}

std::chrono::system_clock::time_point NicoLive::getLiveStartTime() const
{
	return this->live_info.start_time;
}

std::chrono::system_clock::time_point NicoLive::getLiveEndTime() const
{
	return this->live_info.end_time;
}

std::chrono::system_clock::duration NicoLive::getRemainingStartTime() const
{
	return this->live_info.start_time - this->live_info.server_time;
}

std::chrono::system_clock::duration NicoLive::getRemainingEndTime() const
{
	return this->live_info.end_time - this->live_info.server_time;
}

std::chrono::system_clock::duration NicoLive::getRemainingLive() const
{
	if (isOnair())
		return this->live_info.end_time -
		       std::chrono::system_clock::now();
	else
		return std::chrono::milliseconds(0);
}

bool NicoLive::enabledStopBeforeEndTime() const
{
	// TODO: 実装！
	return true;
}

bool NicoLive::enabledStartBeforeStartTime() const
{
	// TODO: 実装！
	return true;
}

bool NicoLive::enabledAdjustBitrate() const
{
	return this->flags.adjust_bitrate;
}

bool NicoLive::enabledSession() const { return this->flags.session_valid; }

bool NicoLive::isOnair() const { return this->flags.onair; }

void NicoLive::startStreaming()
{
	this->onair_live_id = getLiveId();
	this->flags.onair = true;
}

void NicoLive::stopStreaming()
{
	this->onair_live_id = std::string();
	this->flags.onair = false;
}

void NicoLive::startWatching() { this->watcher->start(); }

void NicoLive::stopWatching() { this->watcher->stop(); }

void NicoLive::nextSilentOnce() { this->flags.silent_once = true; }
bool NicoLive::silentOnce()
{
	bool once = this->flags.silent_once;
	this->flags.silent_once = false;
	return once;
}

bool NicoLive::checkSession()
{
	return (sitePubStat() || (siteLoginNLE() && sitePubStat()));
}

bool NicoLive::checkLive() { return siteLiveProf(); }

bool NicoLive::siteLogin()
{
	if (this->mail.empty() || this->password.empty()) {
		nicolive_log_warn("no mail or password");
		return false;
	}

	bool result =
	    this->webApi->loginSiteNicolive(this->mail, this->password);
	if (result) {
		this->session = this->webApi->getCookie("user_session").c_str();
	}
	return result;
}

bool NicoLive::siteLoginNLE()
{
	if (this->mail.empty() || this->password.empty()) {
		nicolive_log_warn("no mail or password");
		return false;
	}

	std::string ticket =
	    this->webApi->loginNicoliveEncoder(this->mail, this->password);
	nicolive_log_debug("ticket: %20s", ticket.c_str());
	if (!ticket.empty()) {
		this->ticket = ticket;
		return true;
	} else {
		return false;
	}
}

bool NicoLive::sitePubStat()
{
	nicolive_log_debug("session: %20s", this->session.c_str());
	nicolive_log_debug("ticket: %20s", this->ticket.c_str());

	bool useTicket = false;
	if (this->session.empty()) {
		if (this->siteLoginNLE()) {
			useTicket = true;
		} else {
			nicolive_log_debug(
			    "this->session and this->ticket"
			    " are both empty.");
			this->flags.onair = false;
			clearLiveInfo();
			return false;
		}
	}

	const std::string statusXpath = "/getpublishstatus/@status";
	const std::string errorCodeXpath =
	    "/getpublishstatus/error/code/text()";
	const std::unordered_map<std::string, std::string> xpathMap = {
	    {"server_time", "/getpublishstatus/@time"},
	    {"id", "/getpublishstatus//stream/id/text()"},
	    {"exclude", "/getpublishstatus//stream/exclude/text()"},
	    {"base_time", "/getpublishstatus//stream/base_time/text()"},
	    {"open_time", "/getpublishstatus//stream/open_time/text()"},
	    {"start_time", "/getpublishstatus//stream/start_time/text()"},
	    {"end_time", "/getpublishstatus//stream/end_time/text()"},
	    {"url", "/getpublishstatus//rtmp/url/text()"},
	    {"stream", "/getpublishstatus//rtmp/stream/text()"},
	    {"ticket", "/getpublishstatus//rtmp/ticket/text()"},
	    {"bitrate", "/getpublishstatus//rtmp/bitrate/text()"},
	};

	std::unordered_map<std::string, std::vector<std::string>> data;
	data[statusXpath] = std::vector<std::string>();
	data[errorCodeXpath] = std::vector<std::string>();
	for (auto &xpathPair : xpathMap) {
		data[xpathPair.second] = std::vector<std::string>();
	}

	bool result = false;
	if (useTicket) {
		result =
		    this->webApi->getPublishStatusTicket(this->ticket, &data);
	} else {
		result = this->webApi->getPublishStatus(&data);
	}

	if (!result) {
		nicolive_log_error("failed get publish status web page");
		return false;
	}

	if (data[statusXpath].empty()) {
		nicolive_log_error("faield get publish status");
		return false;
	}

	bool success = false;
	std::string status = data[statusXpath][0];

	if (status == "ok") {
		try {
			this->live_info.server_time =
			    get_umsvs_time(data, xpathMap, "server_time");
			this->live_info.id =
			    get_umsvs_str(data, xpathMap, "id");
			this->live_info.exclude =
			    get_umsvs_bool(data, xpathMap, "exclude");
			this->live_info.base_time =
			    get_umsvs_time(data, xpathMap, "base_time");
			this->live_info.open_time =
			    get_umsvs_time(data, xpathMap, "open_time");
			this->live_info.start_time =
			    get_umsvs_time(data, xpathMap, "start_time");
			this->live_info.end_time =
			    get_umsvs_time(data, xpathMap, "end_time");
			this->live_info.url =
			    get_umsvs_str(data, xpathMap, "url");
			this->live_info.stream =
			    get_umsvs_str(data, xpathMap, "stream");
			if (data.find(xpathMap.at("ticket")) != data.end() &&
			    data[xpathMap.at("ticket")].size() != 0) {
				this->live_info.ticket =
				    data[xpathMap.at("ticket")].at(0);
			} else {
				this->live_info.ticket = std::string();
			}
			this->live_info.bitrate =
			    get_umsvs_int(data, xpathMap, "bitrate");
			nicolive_log_info(
			    "live waku: %s", this->live_info.id.c_str());
			success = true;
		} catch (const std::out_of_range &err) {
			nicolive_log_error("parse faild: %s", err.what());
			clearLiveInfo();
			success = false;
		} catch (const std::invalid_argument &err) {
			nicolive_log_error("parse faild: %s", err.what());
			clearLiveInfo();
			success = false;
		}
	} else if (status == "fail") {
		clearLiveInfo();
		std::string errorCode = "null";
		if (!data[errorCodeXpath].empty()) {
			errorCode = data[errorCodeXpath][0];
		}
		if (errorCode == "notfound") {
			nicolive_log_info("no live waku");
			success = true;
		} else if (errorCode == "unknown") {
			nicolive_log_warn("login session failed");
		} else {
			nicolive_log_error(
			    "unknow error code: %s", errorCode.c_str());
		}
	} else {
		clearLiveInfo();
		nicolive_log_error("unknow status: %s", status.c_str());
	}

	if (success) {
		this->flags.session_valid = true;
	} else {
		this->flags.session_valid = false;
	}

	return success;
}

bool NicoLive::siteLiveProf()
{

	if (this->live_info.id.empty()) {
		nicolive_log_debug("this->live_info.id is empty.");
		this->live_url = std::string();
		this->live_key = std::string();
		return false;
	} else if (this->live_info.ticket.empty()) {
		nicolive_log_info("found live url and key without ticket");
		this->live_url = std::string();
		this->live_url += this->live_info.url;
		this->live_key = this->live_info.stream;
		return true;
	} else {
		nicolive_log_info("found live url and key with ticket");
		this->live_url = std::string();
		this->live_url += this->live_info.url;
		this->live_url += "?";
		this->live_url += this->live_info.ticket;
		this->live_key = this->live_info.stream;
		return true;
	}
}

bool NicoLive::loadViqoSettings()
{
	const char *session = nicookie_get_session(NICOOKIE_APP_VIQO);
	if (session == nullptr) {
		const char *errMessage = nicookie_strerror(nicookie_errno);
		nicolive_log_error("%s", errMessage);
		return false;
	}
	this->setSession(session);
	return true;

	this->flags.load_viqo = true;
	return true;
}

void NicoLive::clearLiveInfo()
{
	this->live_info = decltype(this->live_info)();
}
