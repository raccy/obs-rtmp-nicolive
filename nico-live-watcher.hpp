#pragma once
#include <chrono>
#include <ctime>
#include <memory>

class NicoLive;
class NicoLiveTimer;

class NicoLiveWatcher
{
public:
	// stopping and starting margin time 2s
	static constexpr auto MARGIN_TIME =
	    std::chrono::system_clock::duration(std::chrono::seconds(2));
	// interval time on air 60s
	static constexpr auto ON_AIR_INTERVAL_TIME =
	    std::chrono::system_clock::duration(std::chrono::seconds(60));
	// interval time off air 10s
	static constexpr auto OFF_AIR_INTERVAL_TIME =
	    std::chrono::system_clock::duration(std::chrono::seconds(10));
	// interval time in boost 2s
	static constexpr auto BOOST_INTERVAL_TIME =
	    std::chrono::system_clock::duration(std::chrono::seconds(2));
	// boost the number of times after stopping
	static const int BOOST_NUMBER_AFTER_STOP = 5;

private:
	NicoLive *nicolive;
	int boostCount = 0;
	std::unique_ptr<NicoLiveTimer> timer;

public:
	NicoLiveWatcher(NicoLive *nicolive);
	~NicoLiveWatcher();
	void start();
	void stop();
	bool isActive();
	// int remainingTime();

private:
	std::chrono::system_clock::duration watch();
	std::chrono::system_clock::duration watchOnAir();
	std::chrono::system_clock::duration watchOffAir();
};
