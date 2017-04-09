#include "nico-live.hpp"
#include <ctime>
#include <curl/curl.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <QtCore>
#include "nicookie.h"
#include "nico-live-api.hpp"
#include "nico-live-watcher.hpp"
#include "nicolive-log.h"
#include "nicolive.h"

NicoLive::NicoLive(QObject *parent)
{
	(void)parent;
	watcher = std::unique_ptr<NicoLiveWatcher>(new NicoLiveWatcher(this));
	webApi = std::unique_ptr<NicoLiveApi>(new NicoLiveApi());
}

NicoLive::~NicoLive() {}

void NicoLive::setSession(const QString &session)
{
	this->session = session;
	this->flags.session_valid = false;
	this->flags.load_viqo = false;
	this->webApi->setCookie("user_session", this->session.toStdString());
}

void NicoLive::setSession(const char *session)
{
	this->setSession(QString(session));
}

void NicoLive::setAccount(const QString &mail, const QString &password)
{
	this->mail = mail;
	this->password = password;
	this->flags.session_valid = false;
	this->flags.load_viqo = false;
}

void NicoLive::setAccount(const char *mail, const char *password)
{
	this->mail = mail;
	this->password = password;
	this->flags.session_valid = false;
	this->flags.load_viqo = false;
}

void NicoLive::setEnabledAdjustBitrate(bool enabled)
{
	this->flags.adjust_bitrate = enabled;
}

const QString &NicoLive::getMail() const { return this->mail; }

const QString &NicoLive::getPassword() const { return this->password; }

const QString &NicoLive::getSession() const { return this->session; }

const QString &NicoLive::getLiveId() const { return this->live_info.id; }

const QString &NicoLive::getLiveUrl() const { return this->live_url; }

const QString &NicoLive::getLiveKey() const { return this->live_key; }

long long NicoLive::getLiveBitrate() const { return this->live_info.bitrate; }

const QString &NicoLive::getOnairLiveId() const { return this->onair_live_id; }

int NicoLive::getRemainingLive() const
{
	if (isOnair())
		return QDateTime::currentDateTime().secsTo(
		    this->live_info.end_time);
	else
		return 0;
}

bool NicoLive::enabledLive() const
{
	QDateTime now = QDateTime::currentDateTime();
	return this->live_info.start_time <= now &&
	       this->live_info.end_time >= now;
}

NicoLive::LiveState NicoLive::getLiveState() const
{
	auto now = time(NULL);
	if (now < getLiveStartTime()) {
		return NicoLive::LiveState::BEFORE_START;
	} else if (now <= getLiveEndTime()) {
		return NicoLive::LiveState::ENABLE_LIVE;
	} else {
		return NicoLive::LiveState::AFTER_END;
	}
}

std::time_t NicoLive::getLiveStartTime() const
{
	return static_cast<std::time_t>(this->live_info.start_time.toTime_t());
}

std::time_t NicoLive::getLiveEndTime() const
{
	return static_cast<std::time_t>(this->live_info.end_time.toTime_t());
}

std::chrono::milliseconds NicoLive::getRemainingStartTime() const
{
	return std::chrono::seconds(this->live_info.start_time.toTime_t() -
				    this->live_info.server_time.toTime_t());
}

std::chrono::milliseconds NicoLive::getRemainingEndTime() const
{
	return std::chrono::seconds(this->live_info.end_time.toTime_t() -
				    this->live_info.server_time.toTime_t());
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
	this->onair_live_id = QString();
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
	if (this->mail.isEmpty() || this->password.isEmpty()) {
		nicolive_log_warn("no mail or password");
		return false;
	}

	bool result = this->webApi->loginSiteNicolive(
	    this->mail.toStdString(), this->password.toStdString());
	if (result) {
		this->session = this->webApi->getCookie("user_session").c_str();
	}
	return result;
}

bool NicoLive::siteLoginNLE()
{
	if (this->mail.isEmpty() || this->password.isEmpty()) {
		nicolive_log_warn("no mail or password");
		return false;
	}

	std::string result = this->webApi->loginNicoliveEncoder(
	    this->mail.toStdString(), this->password.toStdString());
	nicolive_log_debug("ticket: %20s", result.c_str());
	if (!result.empty()) {
		this->ticket = result.c_str();
		return true;
	} else {
		return false;
	}
}

bool NicoLive::sitePubStat()
{
	nicolive_log_debug(
	    "session: %20s", this->session.toStdString().c_str());
	nicolive_log_debug("ticket: %20s", this->ticket.toStdString().c_str());

	bool useTicket = false;
	if (this->session.isEmpty()) {
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
		result = this->webApi->getPublishStatusTicket(
		    this->ticket.toStdString(), &data);
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
			this->live_info.server_time.setTime_t(
			    std::stoi(data[xpathMap.at("server_time")].at(0)));
			this->live_info.id =
			    data[xpathMap.at("id")].at(0).c_str();
			this->live_info.exclude =
			    (data[xpathMap.at("exclude")].at(0) == "1");
			this->live_info.base_time.setTime_t(
			    std::stoi(data[xpathMap.at("base_time")].at(0)));
			this->live_info.open_time.setTime_t(
			    std::stoi(data[xpathMap.at("open_time")].at(0)));
			this->live_info.start_time.setTime_t(
			    std::stoi(data[xpathMap.at("start_time")].at(0)));
			this->live_info.end_time.setTime_t(
			    std::stoi(data[xpathMap.at("end_time")].at(0)));
			this->live_info.url =
			    data[xpathMap.at("url")].at(0).c_str();
			this->live_info.stream =
			    data[xpathMap.at("stream")].at(0).c_str();
			if (data.find(xpathMap.at("ticket")) != data.end() &&
			    data[xpathMap.at("ticket")].size() != 0) {
				this->live_info.ticket =
				    data[xpathMap.at("ticket")].at(0).c_str();
			} else {
				this->live_info.ticket = QString();
			}
			this->live_info.bitrate =
			    std::stoi(data[xpathMap.at("bitrate")].at(0));
			nicolive_log_info("live waku: %s",
			    this->live_info.id.toStdString().c_str());
			success = true;
		} catch (std::out_of_range &err) {
			nicolive_log_error("parse faild?");
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

	if (this->live_info.id.isEmpty()) {
		nicolive_log_debug("this->live_info.id is empty.");
		this->live_url = QString();
		this->live_key = QString();
		return false;
	} else if (this->live_info.ticket.isEmpty()) {
		nicolive_log_info("found live url and key without ticket");
		this->live_url = QString();
		this->live_url += this->live_info.url;
		this->live_key = this->live_info.stream;
		return true;
	} else {
		nicolive_log_info("found live url and key with ticket");
		this->live_url = QString();
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
