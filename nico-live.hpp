#pragma once

#include <chrono>
#include <ctime>
#include <QtCore>

class NicoLiveWatcher;
class NicoLiveApi;

class NicoLive : public QObject
{
	Q_OBJECT
	friend class NicoLiveWatcher;

	enum class LiveState {
		BEFORE_START,
		ENABLE_LIVE,
		AFTER_END,
	};

public:
private:
	QString mail;
	QString password;
	QString session;
	QString ticket;
	struct {
		QString id;
		QString url;
		QString stream;
		QString ticket;
		QDateTime server_time;
		QDateTime base_time;
		QDateTime open_time;
		QDateTime start_time;
		QDateTime end_time;
		long long bitrate = 0;
		bool exclude = false;
	} live_info;
	QString live_url;
	QString live_key;
	QString onair_live_id;
	struct {
		bool session_valid = false;
		bool onair = false;
		bool load_viqo = false;
		bool adjust_bitrate = false;
		bool silent_once = false;
	} flags;
	NicoLiveWatcher *watcher;
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
	bool enabledLive() const;
	LiveState getLiveState() const;
	std::time_t getLiveStartTime() const;
	std::time_t getLiveEndTime() const;
	std::chrono::milliseconds getRemainingStartTime() const;
	std::chrono::milliseconds getRemainingEndTime() const;
	bool enabledStopBeforeEndTime() const;
	bool enabledStartBeforeStartTime() const;

	bool enabledAdjustBitrate() const;
	bool enabledSession() const;
	bool isOnair() const;

	void startStreaming();
	void stopStreaming();
	void startWatching(long long sec = 60);
	void stopWatching();

	bool checkSession();
	bool checkLive();
	bool loadViqoSettings();

	void nextSilentOnce();
	bool silentOnce();

private:
	// Access Niconico Site
	bool siteLogin();
	bool siteLoginNLE();
	bool sitePubStat();
	bool siteLiveProf();

	void clearLiveInfo();
};
