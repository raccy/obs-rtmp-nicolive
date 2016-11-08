#pragma once
#include <chrono>
#include <ctime>

class NicoLive;
class NicoLiveTimer;

class NicoLiveWatcher
{
public:
	// static const int MIN_INTERVAL_SEC = 10;      // 10s
	// static const int MAX_INTERVAL_SEC = 60 * 60; // 1h
	// time is in milliseconds
	// stopping and starting margin time
	static constexpr auto MARGIN_TIME =
	    std::chrono::milliseconds(2 * 1000); // 2s
	// interval time on air
	static constexpr auto ON_AIR_INTERVAL_TIME =
	    std::chrono::milliseconds(60 * 1000); // 60s
	// interval time off air
	static constexpr auto OFF_AIR_INTERVAL_TIME =
	    std::chrono::milliseconds(10 * 1000); // 10s
	// interval time in boost
	static constexpr auto BOOST_INTERVAL_TIME =
	    std::chrono::milliseconds(2 * 1000); // 2s
	// boost the number of times after stopping
	static const int BOOST_NUMBER_AFTER_STOP = 5;

private:
	NicoLive *nicolive;
	// long long marginTime;
	// long long interval = 60 * 1000;
	int boostCount = 0;
	NicoLiveTimer *timer;

public:
	NicoLiveWatcher(NicoLive *nicolive);
	~NicoLiveWatcher();
	void start();
	void stop();
	bool isActive();
	// int remainingTime();

private:
	std::chrono::milliseconds watch();
	std::chrono::milliseconds watchOnAir();
	std::chrono::milliseconds watchOffAir();
};
