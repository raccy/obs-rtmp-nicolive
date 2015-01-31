#pragma once

#include <QtCore>
#include <QtNetwork>

class NicoLive : public QObject {
	// Q_OBJECT
public:
	static const QUrl LOGIN_URL;
	static const QUrl PUBSTAT_URL;
	static const QString FMEPROF_URL_PRE;
private:
	QString mail;
	QString password;
	QString session;
	QString live_id;
	QString live_url;
	QString live_key;
	QNetworkAccessManager* qnam;
public:
	NicoLive();
	void setSession(const char *session);
	void setSession(const QString &session);
	void setAccount(const char *mail, const char *password);
	void setAccount(const QString &mail, const QString &password);
	const QString &getMail() const;
	const QString &getPassword() const;
	const QString &getSession() const;
	const QString &getLiveId() const;
	const QString &getLiveUrl() const;
	const QString &getLiveKey() const;
	bool checkSession();
	bool checkLive();
	bool loadViqoSettings();
private:
	QVariant makeCookieData(const QString &session_id);
	QByteArray getWeb(const QUrl);
	// Access Niconico Site
	bool siteLogin();
	bool sitePubStat();
	bool siteLiveProf();
};
