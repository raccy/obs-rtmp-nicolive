#pragma once

#include <QtCore>

class NicoLive;

class NicoLiveWatcher : public QObject {
	Q_OBJECT
private:
	int merginTime;
	int interval;
	NicoLive *nicolive;
	QTimer *timer;
public:
	NicoLiveWatcher(NicoLive *nicolive, int margin_time = 1000);
	~NicoLiveWatcher();
	void start(int sec);
	void stop();
	bool isActive();
	int remainingTime();
private slots:
	void watch();
};
