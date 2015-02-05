#pragma once

#include <QtCore>
#include <QtNetwork>

class NicoLive : public QObject {
	Q_OBJECT
public:
	static const QUrl LOGIN_URL;
	static const QUrl PUBSTAT_URL;
	static const QString FMEPROF_URL_PRE;
private:
	QString mail;
	QString password;
	QString session;
	struct {
		QString id;
		QString url;
		QString stream;
		QString ticket;
		QDateTime base_time;
		QDateTime open_time;
		QDateTime start_time;
		QDateTime end_time;
		long long bitrate = 0;
		bool exclude = false;
	} live_info;
	// QString live_id;
	QString live_url;
	QString live_key;
	struct {
		bool session_valid = false;
		bool onair = false;
		bool load_viqo = false;
		bool adjust_bitrate = false;
	} flags;
	QNetworkAccessManager* qnam;
public:
	NicoLive(QObject *parent = 0);
	~NicoLive();
	void setSession(const char *session);
	void setSession(const QString &session);
	void setAccount(const char *mail, const char *password);
	void setAccount(const QString &mail, const QString &password);
	void setEnabledAdjustBitrate(bool enabled);
	const QString &getMail() const;
	const QString &getPassword() const;
	const QString &getSession() const;
	const QString &getLiveId() const;
	const QString &getLiveUrl() const;
	const QString &getLiveKey() const;
	long long getLiveBitrate() const;
	bool enabledAdjustBitrate() const;
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
	bool parseXml(QXmlStreamReader &reader, QHash<QString, QString> &hash);
};
