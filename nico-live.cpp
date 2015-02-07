#include <QtCore>
#include <QtNetwork>
#include "nicolive.h"
#include "nico-live.hpp"
#include "nico-live-watcher.hpp"

const QUrl NicoLive::LOGIN_URL =
		QUrl("https://secure.nicovideo.jp/secure/login?site=nicolive");
const QUrl NicoLive::PUBSTAT_URL =
		QUrl("http://live.nicovideo.jp/api/getpublishstatus");
const QString NicoLive::FMEPROF_URL_PRE =
		"http://live.nicovideo.jp/api/getfmeprofile?v=";

NicoLive::NicoLive(QObject *parent)
{
	(void)parent;
	qnam = new QNetworkAccessManager(this);
	watcher = new NicoLiveWatcher(this);
}

NicoLive::~NicoLive()
{
}

void NicoLive::setSession(const QString &session)
{
	this->session = session;
	this->flags.session_valid = false;
	this->flags.load_viqo = false;

}

void NicoLive::setSession(const char *session)
{
	this->session = session;
	this->flags.session_valid = false;
	this->flags.load_viqo = false;
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

const QString &NicoLive::getMail() const
{
	return this->mail;
}

const QString &NicoLive::getPassword() const
{
	return this->password;
}

const QString &NicoLive::getSession() const
{
	return this->session;
}

const QString &NicoLive::getLiveId() const
{
	return this->live_info.id;
}

const QString &NicoLive::getLiveUrl() const
{
	return this->live_url;
}

const QString &NicoLive::getLiveKey() const
{
	return this->live_key;
}

long long NicoLive::getLiveBitrate() const
{
	return this->live_info.bitrate;
}

const QString &NicoLive::getOnairLiveId() const
{
	return this->onair_live_id;
}

int NicoLive::getRemainingLive() const
{
	if (isOnair())
		return QDateTime::currentDateTime().secsTo(
			this->live_info.end_time);
	else
		return 0;
}

bool NicoLive::enabledAdjustBitrate() const
{
	return this->flags.adjust_bitrate;
}

bool NicoLive::enabledSession() const
{
	return this->flags.session_valid;
}

bool NicoLive::isOnair() const
{
	return this->flags.onair;
}

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

void NicoLive::startWatching(int sec)
{
	this->watcher->start(sec);
}

void NicoLive::stopWatching()
{
	this->watcher->stop();
}

bool NicoLive::checkSession()
{
	return sitePubStat() || (siteLogin() && sitePubStat());
}

bool NicoLive::checkLive()
{
	return siteLiveProf();
}

/*
original code by https://github.com/diginatu/Viqo
file: src/NicoLiveManager/nicolivemanager.cpp
Licensed under the MIT License Copyright (c) 2014 diginatu
see https://github.com/diginatu/Viqo/raw/master/LICENSE
*/
QVariant NicoLive::makeCookieData(const QString &session_id)
{
	QVariant cookieData;

	// make cookies
	QList <QNetworkCookie> cookies;
	QNetworkCookie ck;
	ck.toRawForm(QNetworkCookie::NameAndValueOnly);
	ck.setName("user_session");

	QByteArray user_id_ba;
	user_id_ba.append(session_id);

	ck.setValue(user_id_ba);
	cookies.append(ck);

	cookieData.setValue(cookies);
	return cookieData;
}

/*
original code by https://github.com/diginatu/Viqo
file: src/NicoLiveManager/loginapi.cpp
Licensed under the MIT License Copyright (c) 2014 diginatu
see https://github.com/diginatu/Viqo/raw/master/LICENSE
*/
bool NicoLive::siteLogin()
{
	QNetworkRequest rq(NicoLive::LOGIN_URL);
	rq.setHeader(QNetworkRequest::ContentTypeHeader,
			"application/x-www-form-urlencoded");

	QUrlQuery params;
	params.addQueryItem("next_url", "");
	params.addQueryItem("show_button_facebook", "0");
	params.addQueryItem("show_button_twitter", "0");
	params.addQueryItem("nolinks", "0");
	params.addQueryItem("_use_valid_error_code", "0");
	params.addQueryItem("mail", QUrl::toPercentEncoding(this->mail));
	params.addQueryItem("password",
			QUrl::toPercentEncoding(this->password));

	QNetworkReply *netReply = qnam->post(rq,
			params.toString(QUrl::FullyEncoded).toUtf8());

	nicolive_log_info("login start");

	// wait reply
	QEventLoop loop;
	connect(netReply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	nicolive_log_info("login finished");

	// finished reply
	auto headers = netReply->rawHeaderPairs();

	bool success = false;
	foreach (auto header, headers) {
		if (header.first == "Set-Cookie") {
			auto cookies = QNetworkCookie::parseCookies(
					header.second);
			foreach (auto cookie, cookies) {
				if (cookie.name() == "user_session" &&
						cookie.value() != "deleted" &&
						cookie.value() != "") {
					this->session = cookie.value();
					success = true;
					break;
				}
			}
			break;
		}
	}
	netReply->deleteLater();

	if (success) {
		this->flags.session_valid = true;
		nicolive_log_info("login succeeded");
	} else {
		this->flags.session_valid = false;
		nicolive_log_warn("login failed");
	}

	return success;
}

/*
original code by https://github.com/diginatu/Viqo
file: src/NicoLiveManager/rawmylivewaku.cpp
Licensed under the MIT License Copyright (c) 2014 diginatu
see https://github.com/diginatu/Viqo/raw/master/LICENSE
*/
QByteArray NicoLive::getWeb(const QUrl url)
{

	if (this->session.isEmpty())
		return "";

	// make request
	QNetworkRequest rq;
	QVariant cookieData = this->makeCookieData(this->session);
	rq.setHeader(QNetworkRequest::CookieHeader, cookieData);
	rq.setUrl(url);

	QNetworkReply * netReply = qnam->get(rq);

	nicolive_log_info("web get start");

	// wait reply
	QEventLoop loop;
	connect(netReply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	nicolive_log_info("web get finished");

	// finished reply
	QByteArray repdata = netReply->readAll();
	netReply->deleteLater();
	return repdata;
}

bool NicoLive::sitePubStat()
{

	if (this->session.isEmpty()) {
		nicolive_log_debug("this->session is empty.");
		this->flags.onair = false;
		clearLiveInfo();
		return false;
	}

	QXmlStreamReader reader(this->getWeb(NicoLive::PUBSTAT_URL));
	QHash<QString, QString> xml_data;
	if (!parseXml(reader, xml_data)) {
		return false;
	}

	bool success = false;
	QString status = xml_data["/getpublishstatus/@status"];
	QString error_code;

	if (status == "ok") {
		this->flags.onair = true;
		this->live_info.id = xml_data["/getpublishstatus/stream/id"];
		this->live_info.exclude =
			(xml_data["/getpublishstatus/stream/exclude"] == "1");
		this->live_info.base_time.setTime_t(xml_data[
			"/getpublishstatus/stream/base_time"].toUInt());
		this->live_info.open_time.setTime_t(xml_data[
			"/getpublishstatus/stream/open_time"].toUInt());
		this->live_info.start_time.setTime_t(xml_data[
			"/getpublishstatus/stream/start_time"].toUInt());
		this->live_info.end_time.setTime_t(xml_data[
			"/getpublishstatus/stream/end_time"].toUInt());
		this->live_info.url =
			xml_data["/getpublishstatus/rtmp/url"];
		this->live_info.stream =
			xml_data["/getpublishstatus/rtmp/stream"];
		this->live_info.ticket =
			xml_data["/getpublishstatus/rtmp/ticket"];
		this->live_info.bitrate =
			xml_data["/getpublishstatus/rtmp/bitrate"].toInt();
		nicolive_log_info("live waku: %s", this->live_info.id.toStdString().c_str());
		success = true;
	} else if (status == "fail") {
		clearLiveInfo();
		error_code = xml_data["/getpublishstatus/error/code"];
		if (error_code == "notfound") {
			nicolive_log_info("no live waku");
			success = true;
		} else if (error_code == "unknown") {
			nicolive_log_warn("login session failed");
		} else {
			nicolive_log_error("unknow error code: %s",
					error_code.toStdString().c_str());
		}
	} else {
		clearLiveInfo();
		nicolive_log_error("unknow status: %s", status.toStdString().c_str());
	}

	if (success) {
		this->flags.session_valid = true;
	} else {
		this->flags.session_valid = false;
	}

	return success;
}

bool NicoLive::siteLiveProf() {

	if (this->live_info.id.isEmpty()) {
		nicolive_log_debug("this->live_info.id is empty.");
		this->live_url = QString();
		this->live_key = QString();
		return false;
	}

	QString live_prof_url;
	live_prof_url += NicoLive::FMEPROF_URL_PRE;
	live_prof_url += this->live_info.id;

	QXmlStreamReader reader(this->getWeb(QUrl(live_prof_url)));
	QHash<QString, QString> xml_data;
	if (!parseXml(reader, xml_data)) {
		return false;
	}

	this->live_url =
		xml_data["/flashmedialiveencoder_profile/output/rtmp/url"];
	this->live_key =
		xml_data["/flashmedialiveencoder_profile/output/rtmp/stream"];
	if (this->live_url.isEmpty() || this->live_key.isEmpty()) {
		nicolive_log_warn("not found live url or key");
		return false;
	} else {
		nicolive_log_info("found live url and key");
		return true;
	}
}

/*
original code by https://github.com/diginatu/Viqo
file: src/settings.cpp
Licensed under the MIT License Copyright (c) 2014 diginatu
see https://github.com/diginatu/Viqo/raw/master/LICENSE
*/
bool NicoLive::loadViqoSettings()
{
	QStringList dir = QStandardPaths::standardLocations(
			QStandardPaths::DataLocation);
	if (dir.empty()) {
		nicolive_log_error("failed find save directory");
		return false;
	}

	QString viqo_data_dir = dir[0];
	int index_obs = viqo_data_dir.lastIndexOf("obs");
	if (index_obs >= 0) {
		// obs -> Viqo
		viqo_data_dir.replace(index_obs, 3, "Viqo");
	} else {
		nicolive_log_error("found invalid save directory");
		this->flags.load_viqo = false;
		return false;
	}
	nicolive_log_debug("save dir: %s", viqo_data_dir.toStdString().c_str());

	QFile file(viqo_data_dir + "/settings.json");
	if ( !file.exists() ) {
		nicolive_log_warn("viqo save file is not available");
		this->flags.load_viqo = false;
		return false;
	}

	file.open(QIODevice::ReadOnly | QIODevice::Text);

	QJsonDocument jsd = QJsonDocument::fromJson(file.readAll());

	QJsonObject login_way = jsd.object()["login_way"].toObject();
	this->session = login_way["user_session"].toString();

	QJsonObject user_data = jsd.object()["user_data"].toObject();
	this->mail = user_data["mail"].toString();
	this->password = user_data["pass"].toString();

	file.close();
	this->flags.load_viqo = true;
	return true;
}

// This method parse XML and create hash map.
// The keys are XPath strings for all text and attribute values in elements
bool NicoLive::parseXml(QXmlStreamReader &reader, QHash<QString, QString> &hash)
{
	QStringList element_stack;
	QString content;

	auto xpath = [](const QStringList &element_list)->QString{
		QString str;
		for (auto element: element_list)
			(str += "/") += element;
		return str;
	};

	auto xpath_attr = [](const QStringList &element_list,
			const QStringRef &attr_name)->QString{
		QString str;
		for (auto element: element_list)
			(str += "/") += element;
		(str += "/@") += attr_name;
		return str;
	};

	while (!reader.atEnd()) {
		switch (reader.tokenType()) {
		case QXmlStreamReader::StartElement:
			if (!content.isEmpty()) {
				hash[xpath(element_stack)] += content;
				content = QString();
			}
			element_stack.append(reader.name().toString());
			for (auto attr: reader.attributes()) {
				hash[xpath_attr(element_stack, attr.name())] +=
						attr.value().toString();
			}
			break;
		case QXmlStreamReader::EndElement:
			if (!content.isEmpty()) {
				hash[xpath(element_stack)] += content;
				content = QString();
			}
			if (!element_stack.isEmpty())
				element_stack.removeLast();
			else
				nicolive_log_error("found invaild xml: more end element");
			break;
		case QXmlStreamReader::Characters:
			if (!reader.isWhitespace())
				content += reader.text();
			break;
		case QXmlStreamReader::NoToken:
		case QXmlStreamReader::Invalid:
		case QXmlStreamReader::StartDocument:
		case QXmlStreamReader::EndDocument:
		case QXmlStreamReader::Comment:
		case QXmlStreamReader::DTD:
		case QXmlStreamReader::EntityReference:
		case QXmlStreamReader::ProcessingInstruction:
		default:
			break;
		}
		reader.readNext();
	}

#ifdef _DEBUG
	for (auto key: hash.keys()) {
		nicolive_log_debug("xml [%s] =  %s",
				key.toStdString().c_str(),
				hash[key].toStdString().c_str());
	}
#endif

	if (reader.hasError()) {
		nicolive_log_error("faield to parse xml: %s",
				reader.errorString().toStdString().c_str());
		return false;
	} else {
		return true;
	}
}

void NicoLive::clearLiveInfo()
{
	this->live_info = decltype(this->live_info)();
}
