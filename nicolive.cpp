#include <cstdlib>
#include <cstring>
#include <QtCore>
#include <QtNetwork>
#include <obs.h>
#include "nicolive.h"

// class NicoLive
class NicoLive : public QObject
{
	// Q_OBJECT
public:
	static const QUrl LOGIN_URL;
	// static const QUrl MYLIVE_URL;
	static const QUrl PUBSTAT_URL;
	static const QString FMEPROF_URL_PRE;
private:
	QString session;
	QString mail;
	QString password;
	QString live_id;
	QString live_url;
	QString live_key;
	QNetworkAccessManager* qnam;
	char *buff = NULL;
	size_t buff_size = 256;
	const char *buff_str(const char * str);
public:
	NicoLive();
	void setSession(const char *session);
	void setAccount(const char *mail, const char *password);
	const char *getSession();
	const char *getLiveId();
	const char *getLiveUrl(const char *live_id);
	const char *getLiveKey(const char *live_id);
	bool checkSession();
	QVariant makeCookieData(const QString &session_id);
	QByteArray getWeb(const QUrl);
	// Access Niconico Site
	bool siteLogin();
	bool sitePubStat();
	bool siteLiveProf();
};

const QUrl NicoLive::LOGIN_URL =
		QUrl("https://secure.nicovideo.jp/secure/login?site=nicolive");
// const QUrl NicoLive::MYLIVE_URL =
// 		QUrl("http://live.nicovideo.jp/my");
const QUrl NicoLive::PUBSTAT_URL =
		QUrl("http://live.nicovideo.jp/api/getpublishstatus");
const QString NicoLive::FMEPROF_URL_PRE =
		"http://live.nicovideo.jp/api/getfmeprofile?v=";

NicoLive::NicoLive()
{
	qnam = new QNetworkAccessManager(this);
	buff = (char *)malloc(sizeof(char) * buff_size);
	if (buff == NULL) {
		error("failed malloc buff");
	}
}

const char *NicoLive::buff_str(const char *str)
{
	debug_call_func();
	size_t len = strlen(str);
	if (len + 1 > buff_size) {
		buff_size = (len + 1) * 2; // double size !!
		buff = (char *)realloc(buff, sizeof(char) * buff_size);
		if (buff == NULL) {
			error("failed malloc buff");
			return NULL;
		}
	}
	return strcpy(buff, str);
}

void NicoLive::setSession(const char *session)
{
	debug_call_func();
	this->session = session;
}

void NicoLive::setAccount(const char *mail, const char *password)
{
	debug_call_func();
	this->mail = mail;
	this->password = password;
}

const char *NicoLive::getSession()
{
	debug_call_func();
	return buff_str(this->session.toStdString().c_str());
}

const char *NicoLive::getLiveId()
{
	debug_call_func();
	if (this->sitePubStat()) {
		if (this->siteLiveProf()) {
			debug("nioclive.live_id: %s", this->live_id.toStdString().c_str());
			return buff_str(this->live_id.toStdString().c_str());
		}
	}
	return NULL;
}

const char *NicoLive::getLiveUrl(const char *live_id)
{
	debug_call_func();
	debug("check live_id: '%s'", live_id);
	debug("check this->live_id: '%s'", this->live_id.toStdString().c_str());
	debug("same? %d", this->live_id == live_id);
	// if (this->live_id == live_id) {
		debug("nioclive.live_url: %s", this->live_url.toStdString().c_str());
		return buff_str(this->live_url.toStdString().c_str());
	// }
	return NULL;
}

const char *NicoLive::getLiveKey(const char *live_id)
{
	debug_call_func();
	debug("check live_id: '%s'", live_id);
	debug("check this->live_id: '%s'", this->live_id.toStdString().c_str());
	debug("same? %d", this->live_id == live_id);
	// if (this->live_id == live_id) {
		debug("nioclive.live_key: %s", this->live_key.toStdString().c_str());
		return buff_str(this->live_key.toStdString().c_str());
	// }

// NG	Connecting to RTMP URL rtmp://nlpoc
// OK	Connection to rtmp://nlpoca147.
//
	return NULL;
}

bool NicoLive::checkSession()
{
	debug_call_func();
	return this->sitePubStat();
}

/*
original code by https://github.com/diginatu/Viqo
file: src/NicoLiveManager/nicolivemanager.cpp
Licensed under the MIT License Copyright (c) 2014 diginatu
see https://github.com/diginatu/Viqo/raw/master/LICENSE
*/
QVariant NicoLive::makeCookieData(const QString &session_id)
{
	debug_call_func();
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
	debug_call_func();
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
	params.addQueryItem("password", QUrl::toPercentEncoding(this->password));

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
			auto cookies = QNetworkCookie::parseCookies(header.second);
			foreach (auto cookie, cookies) {
				if (cookie.name() == "user_session" &&
						cookie.value() != "deleted" &&
						cookie.value() != "") {
					this->session = cookie.value();
					success = true;
					// info("login succeeded: %s", this->session.toStdString().c_str());
					info("login succeeded: %s", "secret");
					break;
				}
			}
			break;
		}
	}
	if (!success) {
		warn("[nicolive] login failed");
	}

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
	debug_call_func();

	if (this->session.isEmpty()) {
		return "";
	}

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
	debug_call_func();

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
		debug("read token: %s", reader.tokenString().toStdString().c_str());
		if (reader.isStartElement() && reader.name() == "getpublishstatus") {
			status = reader.attributes().value("status").toString();
			if (status == "ok") {
				reader.readNext(); // <stream>
				reader.readNext(); // <id>
				reader.readNext(); // content of code
				this->live_id = reader.text().toString();
				info("live waku: %s", this->live_id.toStdString().c_str());
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
					error("unknow error code: %s", error_code.toStdString().c_str());
				}
				break;
			} else {
				error("unknow status: %s", status.toStdString().c_str());
				break;
			}
		}
		reader.readNext();
	}

	if (reader.hasError()) {
		error("read error: %s", reader.errorString().toStdString().c_str());
	}

	return success;
}

bool NicoLive::siteLiveProf() {
	debug_call_func();

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
		debug("read token: %s", reader.tokenString().toStdString().c_str());
		if (reader.isStartElement() && reader.name() == "rtmp") {
			while (!reader.atEnd()) {
				if (reader.isStartElement() && reader.name() == "url") {
					reader.readNext();
					if (reader.isCharacters()) {
						this->live_url = reader.text().toString();
						this->live_key = this->live_id; // same stream key and live id
						success = true;
						info("found live url");
						break;
					} else {
						error("invalid xml: rtmp->url next not contents");
						break;
					}
				// } else if (reader.isStartElement() && reader.name() == "stream") {
				// 	if (reader.isCharacters()) {
				// 		this->live_key = reader.text().toString();
				// 		success_key = true;
				// 		info("found live key");
				// 		debug("live key: %s", this->live_key.toStdString().c_str());
				// 	} else {
				// 		error("invalid xml: rtmp->stream next not contents");
				// 		break;
				// 	}
				} else if (reader.isEndElement() && reader.name() == "rtmp") {
					error("invalid xml: rtmp end before url");
					break;
				}
			reader.atEnd() || reader.readNext();
			}
			break;
		}
		reader.atEnd() || reader.readNext();
	} // end while

	if (reader.hasError()) {
		error("read error: %s", reader.errorString().toStdString().c_str());
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
// end of NicoLive class

static NicoLive nicolive;

extern "C" bool nicolive_chek_session_n(const char *session)
{
	debug_call_func();
	nicolive.setSession(session);
	return nicolive.checkSession();
}

extern "C" const char *nicolive_get_session(const char *mail,
	const char *password)
{
	debug_call_func();
	nicolive.setAccount(mail, password);
	if (nicolive.siteLogin()) {
		return nicolive.getSession();
	} else {
		return NULL;
	}
}

extern "C" const char *nicolive_get_live_id(const char *session)
{
	debug_call_func();
	nicolive.setSession(session);
	return nicolive.getLiveId();
}

extern "C" const char *nicolive_get_live_url(const char *session,
		const char *live_id)
{
	debug_call_func();
	nicolive.setSession(session);
	return nicolive.getLiveUrl(live_id);
}

extern "C" const char *nicolive_get_live_key(const char *session,
		const char *live_id)
{
	debug_call_func();
	nicolive.setSession(session);
	return nicolive.getLiveKey(live_id);
}
