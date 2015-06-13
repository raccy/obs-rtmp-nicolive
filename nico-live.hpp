#pragma once

#include <QtCore>
// #include <QtNetwork>

class NicoLiveWatcher;
class NicoLiveCmdServer;
class NicoLiveApi;

class NicoLive : public QObject {
	Q_OBJECT
	friend class NicoLiveWatcher;
public:
	static const QUrl LOGIN_URL;
	static const QUrl NLE_LOGIN_URL;
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
	QString onair_live_id;
	struct {
		bool session_valid = false;
		bool onair = false;
		bool load_viqo = false;
		bool adjust_bitrate = false;
		bool cmd_server_started = false;
		bool silent_once = false;
	} flags;
	// QNetworkAccessManager* qnam;
	NicoLiveWatcher *watcher;
	NicoLiveCmdServer *cmd_server;
	NicoLiveApi *webApi;
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
	const QString &getOnairLiveId() const;
	int getRemainingLive() const;

	bool enabledAdjustBitrate() const;
	bool enabledSession() const;
	bool isOnair() const;

	void startStreaming();
	void stopStreaming();
	void startWatching(long long sec = 60);
	void stopWatching();
	bool startCmdServer(long long port = 0);
	void stopCmdServer();

	bool checkSession();
	bool checkLive();
	bool loadViqoSettings();

	void nextSilentOnce();
	bool silentOnce();
private:
	// QVariant makeCookieData(const QString &session_id);
	// QByteArray getWeb(const QUrl);
	// Access Niconico Site
	bool siteLogin();
	// const QString siteLoginNLE(const QString &mail,
	// 		const QString &password) const;
	bool sitePubStat();
	bool siteLiveProf();
	// bool parseXml(QXmlStreamReader &reader, QHash<QString, QString> &hash);
	void clearLiveInfo();
};
