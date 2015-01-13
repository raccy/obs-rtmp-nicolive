#include <QtCore>
#include <QtNetwork>

#include <obs.h>

#include "nicolive.h"

// memo
// http://stackoverflow.com/questions/5486090/qnetworkreply-wait-for-finished

class NicoLive : public QObject
{
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
	QStringList live_list;
	QNetworkAccessManager* mLoginManager;
	QNetworkAccessManager* mRawMyLiveManager;
public:
	NicoLive();
	void set_session(const char *session);
	void set_account(const char *mail, const char *password);
	const char *get_session();
	const char *get_live_id();
	const char *get_live_url(const char *live_id);
	const char *get_live_key(const char *live_id);
	bool check_session();
	bool site_login();
private:
	QVariant makePostData(const QString &session_id);
	// Access Niconico Site
	bool site_live_my();
	bool site_live_prof();
};

const QUrl NicoLive::LOGIN_URL =
		QUrl("https://secure.nicovideo.jp/secure/login?site=nicolive");
const QUrl NicoLive::MYLIVE_URL =
		QUrl("http://live.nicovideo.jp/my");
const QString FMEPROF_URL_PRE =
		"http://live.nicovideo.jp/api/getfmeprofile?v=";

NicoLive::NicoLive()
{
}

void NicoLive::set_session(const char *session)
{
	this->session = session;
}

void NicoLive::set_account(const char *mail, const char *password)
{
	this->mail = mail;
	this->password = password;
}

const char *NicoLive::get_session()
{
	return this->session.toStdString().c_str();
}

const char *NicoLive::get_live_id()
{
	if (this->site_live_my()) {
		if (this->site_live_prof()) {
			return this->live_id.toStdString().c_str();
		}
	}
	return NULL;
}

const char *NicoLive::get_live_url(const char *live_id)
{
	if (this->live_id == live_id) {
		return this->live_url.toStdString().c_str();
	}
	return NULL;
}

const char *NicoLive::get_live_key(const char *live_id)
{
	if (this->live_id == live_id) {
		return this->live_key.toStdString().c_str();
	}
	return NULL;
}

bool NicoLive::check_session()
{
	return this->site_live_my();
}

QVariant NicoLive::makePostData(const QString &session_id)
{
	/*
	original code by https://github.com/diginatu/Viqo
	file: src/NicoLiveManager/nicolivemanager.cpp
	Licensed under the MIT License Copyright (c) 2014 diginatu
	see https://github.com/diginatu/Viqo/raw/master/LICENSE
	*/
	QVariant postData;

	// make cookies
	QList <QNetworkCookie> cookies;
	QNetworkCookie ck;
	ck.toRawForm(QNetworkCookie::NameAndValueOnly);
	ck.setName("user_session");

	QByteArray user_id_ba;
	user_id_ba.append(session_id);

	ck.setValue(user_id_ba);
	cookies.append(ck);

	postData.setValue(cookies);
	return postData;
}

bool NicoLive::site_login()
{
	/*
	original code by https://github.com/diginatu/Viqo
			file: src/NicoLiveManager/loginapi.cpp
	Licensed under the MIT License Copyright (c) 2014 diginatu
	see https://github.com/diginatu/Viqo/raw/master/LICENSE
	*/
	if(mLoginManager!=nullptr) delete mLoginManager;
	mLoginManager = new QNetworkAccessManager(this);

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

	QNetworkReply *netReply = mLoginManager->post(rq,
			params.toString(QUrl::FullyEncoded).toUtf8());

	blog(LOG_INFO, "[nicolive] login start");

	// wait reply
	QEventLoop loop;
	connect(netReply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	blog(LOG_INFO, "[nicolive] login finished");

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
					blog(LOG_INFO, "[nicolive] login succeeded: %20s...",
							this->session.toStdString().c_str());
					break;
				}
			}
			break;
		}
		blog(LOG_WARNING, "[nicolive] login failed");
	}

	netReply->deleteLater();
	return success;
}

bool NicoLive::site_live_my()
{
	// get http://live.nicovideo.jp/my
	/*
	original code by https://github.com/diginatu/Viqo
	file: src/NicoLiveManager/rawmylivewaku.cpp
	Licensed under the MIT License Copyright (c) 2014 diginatu
	see https://github.com/diginatu/Viqo/raw/master/LICENSE
	*/
	if(mRawMyLiveManager!=nullptr) delete mRawMyLiveManager;
	mRawMyLiveManager = new QNetworkAccessManager(this);

	if (this->session.isEmpty()) {
		return false;
	}

	// make request
	QNetworkRequest rq;
	QVariant postData = makePostData(this->session);
	rq.setHeader(QNetworkRequest::CookieHeader, postData);
	rq.setUrl(NicoLive::MYLIVE_URL);

	QNetworkReply * netReply = mRawMyLiveManager->get(rq);

	blog(LOG_INFO, "[nicolive] get my live start");

	// wait reply
	QEventLoop loop;
	connect(netReply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	blog(LOG_INFO, "[nicolive] get my live finished");

	// finished reply
	QByteArray repdata = netReply->readAll();
	QXmlStreamReader reader(repdata);

  QString id_str("id");
	QString class_str("class");
	bool logined = false;
	bool live_onair = false;
	while (!reader.atEnd()) {
		if (reader.isStartElement() && reader.name() == "div" &&
				reader.attributes().value(id_str) == "liveItemsWrap") {
			// <div id="liveItemsWrap">
			blog(LOG_INFO, "[nicolive] session alive");
			logined = true;
			while (!reader.atEnd()) {
				if (reader.isStartElement() && reader.name() == "div") {
					if (reader.attributes().value(class_str) == "liveItemImg") {
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
										reader.attributes().value(class_str) == "liveItemTxt") {
									blog(LOG_WARNING, "[nicolive] invalid html");
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

	netReply->deleteLater();
	return logined;
}

bool NicoLive::site_live_prof() {
	// TODO: 後で
	return false;
}


static NicoLive nicolive;

// static char *nikolive_session;

extern "C" bool nicolive_chek_session_n(const char *session)
{
	nicolive.set_session(session);
	return nicolive.check_session();
}

extern "C" const char *nicolive_get_session(const char *mail,
	const char *password)
{
	nicolive.set_account(mail, password);
	if (nicolive.site_login()) {
		return nicolive.get_session();
	} else {
		return NULL;
	}
}

extern "C" const char *nicolive_get_live_id(const char *session)
{
	nicolive.set_session(session);
	return nicolive.get_live_id();
}

extern "C" const char *nicolive_get_live_url(const char *session,
		const char *live_id)
{
	nicolive.set_session(session);
	return nicolive.get_live_url(live_id);
}

extern "C" const char *nicolive_get_live_key(const char *session,
		const char *live_id)
{
	nicolive.set_session(session);
	return nicolive.get_live_key(live_id);
}
