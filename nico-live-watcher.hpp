#pragma once

#include <QtCore>

class NicoLive;

class NicoLiveWatcher : public QObject {
	Q_OBJECT
public:
	static const int MIN_INTERVAL = 10 * 1000;
private:
	NicoLive *nicolive;
	int marginTime;
	int interval = 60 * 1000;
	bool active = false;
	QTimer *timer;
public:
	NicoLiveWatcher(NicoLive *nicolive, int margin_sec = 10);
	~NicoLiveWatcher();
	void start(int sec = 60);
	void stop();
	bool isActive();
	int remainingTime();
private slots:
	void watch();
};
