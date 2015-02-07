#include <QtCore>
#include "nicolive.h"
#include "nico-live.hpp"
#include "nico-live-watcher.hpp"
#include "nicolive-ui.h"

NicoLiveWatcher::NicoLiveWatcher(NicoLive *nicolive, int mergin_sec) :
	QObject(nicolive),
	nicolive(nicolive),
	merginTime(mergin_sec * 1000)
{
	this->timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(watch()));
}

NicoLiveWatcher::~NicoLiveWatcher()
{
	if (isActive())
		stop();
}

void NicoLiveWatcher::start(int sec)
{
	this->interval = sec * 1000;
	if (this->interval < NicoLiveWatcher::MIN_INTERVAL)
		this->interval = NicoLiveWatcher::MIN_INTERVAL;
	if (!this->timer->isActive()) {
		nicolive_log_debug("check session before timer start");
		nicolive->checkSession();
		nicolive_log_debug("start watch, interval: %d", this->interval);
		this->timer->start(this->interval);
	}
}

void NicoLiveWatcher::stop()
{
	if (this->timer->isActive()) {
		nicolive_log_debug("stop watch ");
		this->timer->stop();
	}
}

bool NicoLiveWatcher::isActive()
{
	return this->timer->isActive();
}

int NicoLiveWatcher::remainingTime()
{
	return this->timer->remainingTime() / 1000;
}

void NicoLiveWatcher::watch()
{
	nicolive_log_debug("watching!");
	if (interval != this->timer->interval())
		this->timer->start(interval);

	if (!nicolive->enabledSession()) {
		nicolive_log_debug("no session watch end");
		return;
	}

	nicolive->sitePubStat();
	int remaining_sec = nicolive->getRemainingLive();
	if (nicolive->getLiveId().isEmpty()) {
		if (nicolive->isOnair())
			nicolive_streaming_click();
	} else {
		if (nicolive->getLiveId() != nicolive->getOnairLiveId()) {
			if (nicolive->isOnair())
				nicolive_streaming_click();
			nicolive_streaming_click();
		} else if (remaining_sec < remainingTime()) {
			if (remaining_sec > 0)
				this->timer->start(
					remaining_sec * 1000 + merginTime);
			else
				this->timer->start(merginTime);
		}
	}
}
