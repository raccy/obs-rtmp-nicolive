#include "nico-live.hpp"
#include <QtCore>
#include "nico-live-watcher.hpp"
#include "nicolive-log.h"
#include "nicolive-operation.h"
#include "nicolive.h"

NicoLiveWatcher::NicoLiveWatcher(NicoLive *nicolive, int margin_sec)
    : QObject(nicolive), nicolive(nicolive), marginTime(margin_sec * 1000)
{
	this->timer = new QTimer(this);
	this->timer->setSingleShot(true);
	connect(timer, SIGNAL(timeout()), this, SLOT(watch()));
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
	this->interval = static_cast<int>(sec * 1000);

	if (!this->timer->isActive()) {
		nicolive_log_debug("check session before timer start");
		nicolive->checkSession();
		nicolive_log_debug("start watch, interval: %d", this->interval);
		// this->timer->start(this->interval);
		this->timer->start(this->marginTime);
	}
	this->active = true;
}

void NicoLiveWatcher::stop()
{
	if (this->timer->isActive()) {
		nicolive_log_debug("stop watch ");
		this->timer->stop();
	}
	this->active = false;
}

bool NicoLiveWatcher::isActive() { return this->active; }

int NicoLiveWatcher::remainingTime()
{
	return this->timer->remainingTime() / 1000;
}

void NicoLiveWatcher::watch()
{
	nicolive_log_debug("watching!");

	int next_interval = this->interval;
	int remaining_msec;

	nicolive->sitePubStat();
	remaining_msec = nicolive->getRemainingLive() * 1000;
	if (remaining_msec < 0) remaining_msec = 0;

	if (nicolive->getLiveId().isEmpty()) {
		if (nicolive->isOnair()) {
			nicolive_log_debug("stop streaming because live end");
			nicolive_streaming_stop();
			next_interval = this->marginTime;
		}
	} else {
		if (nicolive->getLiveId() != nicolive->getOnairLiveId()) {
			if (nicolive->isOnair()) {
				nicolive_log_debug(
						"stop streaming for restart");
				nicolive_streaming_stop();
				QThread::sleep(1); // sleep 1 sec
			}
			nicolive_log_debug("start streaming for next live");
			nicolive_streaming_start();
		} else if (remaining_msec + this->marginTime < next_interval) {
			next_interval = remaining_msec + this->marginTime;
		}
	}
	this->timer->start(next_interval);
}
