#include "nico-live-watcher.hpp"
#include <algorithm>
#include <chrono>
// #include <ctime>
#include "nico-live-timer.hpp"
#include "nico-live.hpp"
#include "nicolive-log.h"
#include "nicolive-operation.h"
#include "nicolive.h"

// static consntexpr
constexpr std::chrono::milliseconds NicoLiveWatcher::MARGIN_TIME;
constexpr std::chrono::milliseconds NicoLiveWatcher::ON_AIR_INTERVAL_TIME;
constexpr std::chrono::milliseconds NicoLiveWatcher::OFF_AIR_INTERVAL_TIME;
constexpr std::chrono::milliseconds NicoLiveWatcher::BOOST_INTERVAL_TIME;

NicoLiveWatcher::NicoLiveWatcher(NicoLive *nicolive) : nicolive(nicolive)
{
	this->timer = new NicoLiveTimer([this]() { return this->watch(); },
	    std::chrono::milliseconds(1000));
}

NicoLiveWatcher::~NicoLiveWatcher()
{
	if (isActive()) stop();
}

void NicoLiveWatcher::start()
{
	// if (sec < NicoLiveWatcher::MIN_INTERVAL_SEC)
	// 	sec = NicoLiveWatcher::MIN_INTERVAL_SEC;
	// else if (sec > NicoLiveWatcher::MAX_INTERVAL_SEC)
	// 	sec = NicoLiveWatcher::MAX_INTERVAL_SEC;
	// this->interval = sec * 1000;
	nicolive_log_debug("start watch");
	this->boostCount = 0;
	// this->timer->SetIntervalMsec(this->interval);
	this->timer->Start();
}

void NicoLiveWatcher::stop()
{
	nicolive_log_debug("stop watch ");
	this->timer->Stop();
}

bool NicoLiveWatcher::isActive() { return this->timer->IsActive(); }

std::chrono::milliseconds NicoLiveWatcher::watch()
{
	nicolive_log_debug(
	    "watching! since epoch (ms) %lld",
	    static_cast<long long>(
		std::chrono::duration_cast<std::chrono::milliseconds>(
		    std::chrono::system_clock::now().time_since_epoch())
		    .count()));
	if (this->boostCount > 0) {
		this->boostCount--;
	}

	this->nicolive->sitePubStat();

	if (this->nicolive->isOnair()) {
		return this->watchOnAir();
	} else {
		return this->watchOffAir();
	}
}

std::chrono::milliseconds NicoLiveWatcher::watchOnAir()
{
	if (!this->nicolive->enabledLive() ||
	    this->nicolive->getLiveId() != this->nicolive->getOnairLiveId()) {
		nicolive_streaming_stop();
		this->boostCount = NicoLiveWatcher::BOOST_NUMBER_AFTER_STOP;
		return NicoLiveWatcher::MARGIN_TIME;
	}

	auto remainingEndTime = this->nicolive->getRemainingEndTime();

	if (this->nicolive->enabledStopBeforeEndTime()) {
		if (remainingEndTime <= NicoLiveWatcher::MARGIN_TIME) {
			nicolive_streaming_stop();
			this->boostCount =
			    NicoLiveWatcher::BOOST_NUMBER_AFTER_STOP;
			return NicoLiveWatcher::MARGIN_TIME;
		} else {
			return std::min(NicoLiveWatcher::ON_AIR_INTERVAL_TIME,
			    remainingEndTime - NicoLiveWatcher::MARGIN_TIME);
		}
	}

	return std::min(NicoLiveWatcher::ON_AIR_INTERVAL_TIME,
	    remainingEndTime + NicoLiveWatcher::MARGIN_TIME);
}

std::chrono::milliseconds NicoLiveWatcher::watchOffAir()
{
	if (!this->nicolive->enabledLive()) {
		if (this->boostCount > 0) {
			return NicoLiveWatcher::BOOST_INTERVAL_TIME;
		} else {
			return NicoLiveWatcher::OFF_AIR_INTERVAL_TIME;
		}
	}

	if (!this->nicolive->enabledStartBeforeStartTime()) {
		auto remainingStartTime =
		    this->nicolive->getRemainingStartTime();
		if (remainingStartTime < std::chrono::milliseconds::zero()) {
			return std::min(NicoLiveWatcher::OFF_AIR_INTERVAL_TIME,
			    remainingStartTime);
		}
	}

	return NicoLiveWatcher::OFF_AIR_INTERVAL_TIME;
}
