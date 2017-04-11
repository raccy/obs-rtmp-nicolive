#pragma once

#include <chrono>
#include <ctime>
#include <memory>
#include <string>

class NicoLiveWatcher;
class NicoLiveApi;

class NicoLive
{
	friend class NicoLiveWatcher;

	enum class LiveState {
		BEFORE_START,
		ENABLE_LIVE,
		AFTER_END,
	};

public:
private:
	std::string mail;
	std::string password;
	std::string session;
	std::string ticket;
	struct {
		std::string id;
		std::string url;
		std::string stream;
		std::string ticket;
		std::chrono::system_clock::time_point server_time;
		std::chrono::system_clock::time_point base_time;
		std::chrono::system_clock::time_point open_time;
		std::chrono::system_clock::time_point start_time;
		std::chrono::system_clock::time_point end_time;
		long long bitrate = 0;
		bool exclude = false;
	} live_info;
	std::string live_url;
	std::string live_key;
	std::string onair_live_id;
	struct {
		bool session_valid = false;
		bool onair = false;
		bool load_viqo = false;
		bool adjust_bitrate = false;
		bool silent_once = false;
	} flags;
	std::unique_ptr<NicoLiveWatcher> watcher;
	std::unique_ptr<NicoLiveApi> webApi;

public:
	NicoLive();
	~NicoLive();
	void setSession(const char *session);
	void setSession(const std::string &session);
	void setAccount(const char *mail, const char *password);
	void setAccount(const std::string &mail, const std::string &password);
	void setEnabledAdjustBitrate(bool enabled);

	const std::string &getMail() const;
	const std::string &getPassword() const;
	const std::string &getSession() const;
	const std::string &getLiveId() const;
	const std::string &getLiveUrl() const;
	const std::string &getLiveKey() const;
	long long getLiveBitrate() const;
	const std::string &getOnairLiveId() const;
	bool enabledLive() const;
	LiveState getLiveState() const;
	std::chrono::system_clock::time_point getLiveStartTime() const;
	std::chrono::system_clock::time_point getLiveEndTime() const;
	std::chrono::system_clock::duration getRemainingStartTime() const;
	std::chrono::system_clock::duration getRemainingEndTime() const;
	std::chrono::system_clock::duration getRemainingLive() const;
	bool enabledStopBeforeEndTime() const;
	bool enabledStartBeforeStartTime() const;

	bool enabledAdjustBitrate() const;
	bool enabledSession() const;
	bool isOnair() const;

	void startStreaming();
	void stopStreaming();
	void startWatching();
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
