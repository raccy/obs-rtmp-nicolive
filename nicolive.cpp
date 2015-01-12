#include "nicolive.h"
#include <QtCore>
#include <QtNetwork>
#include <QtXml>

// memo
// http://stackoverflow.com/questions/5486090/qnetworkreply-wait-for-finished

class NicoLive : public QObject
{
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
	const char *get_live_url(const QString &live_id);
	const char *get_live_key(const char *live_id);
	const char *get_live_key(const QSTring &live_id);
	bool check_session();
	bool login();
	bool get_live_list();
	const QString &get_live_owner(const QString &live_id);
	QVariant makePostData(const QString &session_id);
};

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
	// TODO: あとで
	return this->session.toStdString().c_str();
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
	// https://secure.nicovideo.jp/secure/session
	// http://live.nicovideo.jp/my
	// TODO: あとで
	return this->get_live_list();
}

bool NicoLive::login()
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

	// wait reply
	QEventLoop loop;
	connect(netReply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

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
					break;
				}
			}
			break;
		}
	}

	netReply->deleteLater();
	return success;
}

bool NicoLive::get_live_list()
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

	if (!this->session || this->session == "") {
		return false;
	}

	// make request
	QNetworkRequest rq;
	QVariant postData = makePostData(this->session);
	rq.setHeader(QNetworkRequest::CookieHeader, postData);
	rq.setUrl(QUrl("http://live.nicovideo.jp/my"));

	QNetworkReply * netRply = mRawMyLiveManager->get(rq);

	// wait reply
	QEventLoop loop;
	connect(netReply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	// finished reply
	QByteArray repdata = netReply->readAll();

	StrAbstractor liveID(repdata);

	// seek to Programs from the joined channels/communities if exist
	if (liveID.forwardStr("<div class=\"articleBody \" id=\"ch\">") == -1) {
		mwin->insLog("no joined channels/communities\n");
		return;
	}

	QString ID;
	QString community;
	while((community = liveID.midStr("http://com.nicovideo.jp/community/co", "\">")) != "") {
		ID = liveID.midStr("http://live.nicovideo.jp/watch/lv", "?ref=");

		// if ID contains no number charactor, it is not ID
		bool isID = true;
		for(int i = 0; i < ID.size(); ++i) {
			if( ID[i] > '9' || ID[i] < '0' ) {
				isID = false;
				break;
			}
		}
		if ( !isID ) continue;

		insertLiveWakuList(new LiveWaku(mwin, this, ID, community, this));
		mwin->insLog("added lv" + ID + " to the comunity list");
	}

	mwin->insLog();
	reply->deleteLater();



	return ;
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
	if (nicolive.login()) {
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
