#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

class NicoLiveTimer
{
	const std::function<std::chrono::milliseconds(void)> callable;
	std::chrono::milliseconds minInterval;

	std::atomic_bool active;
	std::atomic_int loopId;
	std::shared_ptr<std::mutex> loopMutex;
	std::shared_ptr<std::atomic_bool> alive;

public:
	NicoLiveTimer() = delete;
	NicoLiveTimer(std::function<std::chrono::milliseconds(void)> callable,
	    std::chrono::milliseconds minInterval = std::chrono::milliseconds(
		0));
	~NicoLiveTimer();
	void Start();
	void Stop();
	bool IsActive() const;

private:
	static void Loop(int id, NicoLiveTimer *timer);
};
