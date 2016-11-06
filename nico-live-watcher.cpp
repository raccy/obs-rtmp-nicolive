#include "nico-live-watcher.hpp"
#include <ctime>
#include "nico-live-timer.hpp"
#include "nico-live.hpp"
#include "nicolive-log.h"
#include "nicolive-operation.h"
#include "nicolive.h"

NicoLiveWatcher::NicoLiveWatcher(NicoLive *nicolive, int margin_sec)
    : nicolive(nicolive), marginTime(margin_sec * 1000)
{
	this->timer = new NicoLiveTimer(
	    this->interval, [this](std::time_t t) { return this->watch(t); });
}

NicoLiveWatcher::~NicoLiveWatcher()
{
	if (isActive()) stop();
}

void NicoLiveWatcher::start(long long sec)
{
	if (sec < NicoLiveWatcher::MIN_INTERVAL_SEC)
		sec = NicoLiveWatcher::MIN_INTERVAL_SEC;
	else if (sec > NicoLiveWatcher::MAX_INTERVAL_SEC)
		sec = NicoLiveWatcher::MAX_INTERVAL_SEC;
	this->interval = sec * 1000;
	nicolive_log_debug("start watch, interval: %lld", this->interval);
	this->timer->SetIntervalMsec(this->interval);
	this->timer->Start();
}

void NicoLiveWatcher::stop()
{
	nicolive_log_debug("stop watch ");
	this->timer->Stop();
}

bool NicoLiveWatcher::isActive() { return this->timer->IsActive(); }

std::time_t NicoLiveWatcher::watch(std::time_t t)
{
	// TODO: 再考！！！！
	nicolive_log_debug("watching! %lld", static_cast<long long>(t));
	t += this->interval / 1000;

	this->nicolive->sitePubStat();
	auto liveId = this->nicolive->getLiveId();
	if (liveId.isEmpty()) {
		this->stopStreaming();
	} else if (liveId != nicolive->getOnairLiveId()) {
		this->stopStreaming();
		// wait... 1 sec
		t += 1;
	} else {
		switch (nicolive->getLiveState()) {
		case NicoLive::LiveState::BEFORE_START:
			t = this->nicolive->getLiveStartTime();
			break;
		case NicoLive::LiveState::ENABLE_LIVE:
			t = this->nicolive->getLiveEndTime();
			break;
		case NicoLive::LiveState::AFTER_END:
			this->stopStreaming();
			break;
		}
	}
	return t;
}

void NicoLiveWatcher::startStreaming()
{
	if (!this->nicolive->isOnair()) nicolive_streaming_start();
}

void NicoLiveWatcher::stopStreaming()
{
	if (this->nicolive->isOnair()) nicolive_streaming_stop();
}
