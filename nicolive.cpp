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
	static const QUrl MYLIVE_URL;
	static const QString FMEPROF_URL_PRE;
private:
	QString session;
	QString mail;
	QString password;
	QString live_id;
	QString live_url;
	QString live_key;
	QNetworkAccessManager* qnam;
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
	bool siteLiveMy();
	bool siteLiveProf();
};

const QUrl NicoLive::LOGIN_URL =
		QUrl("https://secure.nicovideo.jp/secure/login?site=nicolive");
const QUrl NicoLive::MYLIVE_URL =
		QUrl("http://live.nicovideo.jp/my");
const QString NicoLive::FMEPROF_URL_PRE =
		"http://live.nicovideo.jp/api/getfmeprofile?v=";

NicoLive::NicoLive()
{
	qnam = new QNetworkAccessManager(this);
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
	return this->session.toStdString().c_str();
}

const char *NicoLive::getLiveId()
{
	debug_call_func();
	if (this->siteLiveMy()) {
		if (this->siteLiveProf()) {
			return this->live_id.toStdString().c_str();
		}
	}
	return NULL;
}

const char *NicoLive::getLiveUrl(const char *live_id)
{
	debug_call_func();
	if (this->live_id == live_id) {
		return this->live_url.toStdString().c_str();
	}
	return NULL;
}

const char *NicoLive::getLiveKey(const char *live_id)
{
	debug_call_func();
	if (this->live_id == live_id) {
		return this->live_key.toStdString().c_str();
	}
	return NULL;
}

bool NicoLive::checkSession()
{
	debug_call_func();
	return this->siteLiveMy();
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
	QNetworkRequest rq(QUrl(
			"https://secure.nicovideo.jp/secure/login?site=nicolive"));
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
					info("login succeeded: %s", this->session.toStdString().c_str());
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

bool NicoLive::siteLiveMy()
{
	debug_call_func();

	if (this->session.isEmpty()) {
		this->live_id = QString();
		return false;
	}

	QXmlStreamReader reader(this->getWeb(NicoLive::MYLIVE_URL));

	bool success = false;
	bool live_onair = false;
	while (!reader.atEnd()) {
		if (reader.isStartElement() && reader.name() == "div" &&
				reader.attributes().value("id") == "liveItemsWrap") {
			// <div id="liveItemsWrap">
			info("[nicolive] session alive");
			success = true;
			while (!reader.atEnd()) {
				if (reader.isStartElement() && reader.name() == "div") {
					if (reader.attributes().value("class") == "liveItemImg") {
						while (!reader.atEnd()) {
							if (reader.isStartElement()) {
								if (reader.name() == "img") {
									if (reader.attributes().value("alt") == "ONAIR") {
										live_onair = true;
									} else {
										// not onair
										break;
									}
								} else if (live_onair && reader.name() == "a") {
									// strlen("http://live.nicovideo.jp/editstream/") == 36
									this->live_id =
											reader.attributes().value("href").mid(36).toString();
									break;
								} else if (reader.name() == "a" &&
										reader.attributes().value("class") == "liveItemTxt") {
									warn("invalid html");
									break;
								}
							}
							reader.atEnd() || reader.readNext();
						} // end while
					} else if (reader.attributes().value("class") == "pager") {
						break;
					}
				}
				reader.atEnd() || reader.readNext();
			} // end while
			break;
		}
		reader.atEnd() || reader.readNext();
	} // end while

	if (!live_onair) {
		this->live_id = QString();
	}

	return success;
}

bool NicoLive::siteLiveProf() {
	debug_call_func();

	if (this->live_id.isEmpty()) {
		this->live_url = QString();
		this->live_key = QString();
		return false;
	}

	QString live_prof_url;
	live_prof_url += NicoLive::FMEPROF_URL_PRE;
	live_prof_url += this->live_id;

	QXmlStreamReader reader(this->getWeb(QUrl(live_prof_url)));

	bool success_url = false;
	bool success_key = false;
	while (!reader.atEnd()) {
		if (reader.isStartElement() && reader.name() == "rtmp") {
			while (!reader.atEnd()) {
				if (reader.isStartElement() && reader.name() == "url") {
					reader.readNext();
					if (reader.isCharacters()) {
						this->live_url = reader.text().toString();
						success_url = true;
					} else {
						error("invalid xml: rtmp->url next not contents");
						break;
					}
				} else if (reader.isStartElement() && reader.name() == "stream") {
					if (reader.isCharacters()) {
						this->live_key = reader.text().toString();
						success_key = true;
					} else {
						error("invalid xml: rtmp->stream next not contents");
						break;
					}
				} else if (reader.isEndElement() && reader.name() == "rtmp") {
					break;
				}
			reader.atEnd() || reader.readNext();
			}
			break;
		}
		reader.atEnd() || reader.readNext();
	} // end while

	if (success_url && success_key) {
		return true;
	} else {
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
