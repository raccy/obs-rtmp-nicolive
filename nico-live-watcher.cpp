#include <QtCore>
#include "nicolive.h"
#include "nico-live.hpp"
#include "nico-live-watcher.hpp"
#include "nicolive-ui.h"

NicoLiveWatcher::NicoLiveWatcher(NicoLive *nicolive, int margin_time) :
	QObject(nicolive),
	merginTime(margin_time)
{
	this->nicolive = nicolive;
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
	this->timer->start(interval);
}

void NicoLiveWatcher::stop()
{
	this->timer->stop();
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
	if (interval != this->timer->interval())
		this->timer->start(interval);

	if (!nicolive->enabledSession()) {
		return;
	}

	nicolive->sitePubStat();
	int remaining_time = nicolive->getRemainingLive();
	if (nicolive->getLiveId().isEmpty()) {
		if (nicolive->isOnair())
			nicolive_streaming_click();
	} else {
		if (nicolive->getLiveId() != nicolive->getOnairLiveId()) {
			if (nicolive->isOnair())
				nicolive_streaming_click();
			nicolive_streaming_click();
		} else if (remaining_time < remainingTime()) {
			if (remaining_time > 0)
				start(remaining_time + merginTime);
			else
				start(merginTime);
		}
	}
}
