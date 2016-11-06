#pragma once
#include <ctime>

class NicoLive;
class NicoLiveTimer;

class NicoLiveWatcher
{
public:
	static const int MIN_INTERVAL_SEC = 10;      // 10s
	static const int MAX_INTERVAL_SEC = 60 * 60; // 1h
private:
	NicoLive *nicolive;
	long long marginTime;
	long long interval = 60 * 1000;
	NicoLiveTimer *timer;

public:
	NicoLiveWatcher(NicoLive *nicolive, int margin_sec = 10);
	~NicoLiveWatcher();
	void start(long long sec = 60);
	void stop();
	bool isActive();
	int remainingTime();

private:
	std::time_t watch(std::time_t);
	void startStreaming();
	void stopStreaming();
};
