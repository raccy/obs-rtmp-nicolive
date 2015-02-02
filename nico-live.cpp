#include <cstdlib>
#include <cstring>
#include <QtCore>
#include <QtNetwork>
#include <obs.h>
#include "nicolive.h"
#include "nico-live.hpp"

const QUrl NicoLive::LOGIN_URL =
		QUrl("https://secure.nicovideo.jp/secure/login?site=nicolive");
const QUrl NicoLive::PUBSTAT_URL =
		QUrl("http://live.nicovideo.jp/api/getpublishstatus");
const QString NicoLive::FMEPROF_URL_PRE =
		"http://live.nicovideo.jp/api/getfmeprofile?v=";

NicoLive::NicoLive()
{
	qnam = new QNetworkAccessManager(this);
}

void NicoLive::setSession(const QString &session)
{
	this->session = session;
}

void NicoLive::setSession(const char *session)
{
	this->session = session;
}

void NicoLive::setAccount(const QString &mail, const QString &password)
{
	this->mail = mail;
	this->password = password;
}

void NicoLive::setAccount(const char *mail, const char *password)
{
	this->mail = mail;
	this->password = password;
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
	return this->live_id;
}

const QString &NicoLive::getLiveUrl() const
{
	return this->live_url;
}

const QString &NicoLive::getLiveKey() const
{
	return this->live_key;
}

bool NicoLive::checkSession()
{
	return sitePubStat() || siteLogin();
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

	info("login start");

	// wait reply
	QEventLoop loop;
	connect(netReply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	info("login finished");

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
					info("login succeeded: %s", "secret");
					break;
				}
			}
			break;
		}
	}
	if (!success)
		warn("[nicolive] login failed");

	netReply->deleteLater();
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

	info("web get start");

	// wait reply
	QEventLoop loop;
	connect(netReply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	info("web get finished");

	// finished reply
	QByteArray repdata = netReply->readAll();
	netReply->deleteLater();
	return repdata;
}

bool NicoLive::sitePubStat()
{

	if (this->session.isEmpty()) {
		debug("this->session is empty.");
		this->live_id = QString();
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
		this->live_id = xml_data["/getpublishstatus/stream/id"];
		info("live waku: %s", this->live_id.toStdString().c_str());
		success = true;
	} else if (status == "fail") {
		error_code = xml_data["/getpublishstatus/error/code"];
		if (error_code == "notfound") {
			info("no live waku");
			success = true;
		} else if (error_code == "unknown") {
			warn("login session failed");
		} else {
			error("unknow error code: %s",
					error_code.toStdString().c_str());
		}

	} else {
		error("unknow status: %s", status.toStdString().c_str());
	}
	return success;
}

bool NicoLive::siteLiveProf() {

	if (this->live_id.isEmpty()) {
		debug("this->live_id is empty.");
		this->live_url = QString();
		this->live_key = QString();
		return false;
	}

	QString live_prof_url;
	live_prof_url += NicoLive::FMEPROF_URL_PRE;
	live_prof_url += this->live_id;

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
		warn("not found live url or key");
		return false;
	} else {
		info("found live url and key");
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
		error("failed find save directory");
		return false;
	}

	QString viqo_data_dir = dir[0];
	int index_obs = viqo_data_dir.lastIndexOf("obs");
	if (index_obs >= 0) {
		// obs -> Viqo
		viqo_data_dir.replace(index_obs, 3, "Viqo");
	} else {
		error("found invalid save directory");
		return false;
	}
	debug("save dir: %s", viqo_data_dir.toStdString().c_str());

	QFile file(viqo_data_dir + "/settings.json");
	if ( !file.exists() ) {
		file.close();
		warn("viqo save file is not available");
		return false;
	}

	file.open(QIODevice::ReadOnly | QIODevice::Text);

	QJsonDocument jsd = QJsonDocument::fromJson(file.readAll());

	QJsonObject login_way = jsd.object()["login_way"].toObject();
	// loginWay = login_way["login_way"].toInt();
	session = login_way["user_session"].toString();
	// cookieFile = login_way["cookie_file_name"].toString();

	QJsonObject user_data = jsd.object()["user_data"].toObject();
	mail = user_data["mail"].toString();
	password = user_data["pass"].toString();

	// QJsonObject comment = jsd.object()["comment"].toObject();
	// if (comment.contains("owner_comment"))
	// 	ownerComment = comment["owner_comment"].toBool();
	// if (comment.contains("viewNG"))
	// 	ownerComment = comment["viewNG"].toBool();

	file.close();
	return true;
}

// This method parse XML and create hash map.
// The keys ars XPath strings for all text and attribute values in elements
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
				error("found invaild xml: more end element");
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
		debug("xml [%s] =  %s",
				key.toStdString().c_str(),
				hash[key].toStdString().c_str());
	}
#endif

	if (reader.hasError()) {
		error("faield to parse xml: %s",
				reader.errorString().toStdString().c_str());
		return false;
	} else {
		return true;
	}
}
