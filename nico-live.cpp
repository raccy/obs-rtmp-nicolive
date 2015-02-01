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

	bool success = false;
	this->live_id = QString();
	QString status;
	QString error_code;
	while (!reader.atEnd()) {
		debug("read token: %s",
				reader.tokenString().toStdString().c_str());
		if (reader.isStartElement() &&
				reader.name() == "getpublishstatus") {
			status = reader.attributes().value("status").toString();
			if (status == "ok") {
				reader.readNext(); // <stream>
				reader.readNext(); // <id>
				reader.readNext(); // content of code
				this->live_id = reader.text().toString();
				info("live waku: %s", this->live_id
						.toStdString().c_str());
				success = true;
				break;
			} else if (status == "fail") {
				reader.readNext(); // <error>
				reader.readNext(); // <code>
				reader.readNext(); // content of code
				error_code = reader.text().toString();
				if (error_code == "notfound") {
					info("no live waku");
					success = true;
				} else if (error_code == "unknown") {
					warn("login session failed");
				} else {
					error("unknow error code: %s",
							error_code.toStdString()
							.c_str());
				}
				break;
			} else {
				error("unknow status: %s",
						status.toStdString().c_str());
				break;
			}
		}
		reader.readNext();
	}

	if (reader.hasError()) {
		error("read error: %s",
				reader.errorString().toStdString().c_str());
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

	bool success = false;
	while (!reader.atEnd()) {
		debug("read token: %s",
				reader.tokenString().toStdString().c_str());
		if (reader.isStartElement() && reader.name() == "rtmp") {
			while (!reader.atEnd()) {
				if (reader.isStartElement() &&
						reader.name() == "url") {
					reader.readNext();
					if (reader.isCharacters()) {
						this->live_url = reader.text()
								.toString();
						// same stream key and live id
						this->live_key = this->live_id;
						success = true;
						info("found live url");
						break;
					} else {
						error("invalid xml: "
								"rtmp->url "
								"next not "
								"contents");
						break;
					}
				} else if (reader.isEndElement() &&
						reader.name() == "rtmp") {
					error("invalid xml: "
							"rtmp end before url");
					break;
				}
			reader.atEnd() || reader.readNext();
			}
			break;
		}
		reader.atEnd() || reader.readNext();
	} // end while

	if (reader.hasError()) {
		error("read error: %s",
				reader.errorString().toStdString().c_str());
	}

	if (success) {
		return true;
	} else {
		warn("not found rtmp url");
		this->live_url = QString();
		this->live_key = QString();
		return false;
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
