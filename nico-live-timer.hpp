#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

class NicoLiveTimer
{
	const std::function<std::chrono::system_clock::duration(void)> callable;
	std::chrono::system_clock::duration minInterval;

	std::atomic_bool active;
	std::atomic_int loopId;
	std::shared_ptr<std::mutex> loopMutex;
	std::shared_ptr<std::atomic_bool> alive;

public:
	NicoLiveTimer() = delete;
	NicoLiveTimer(
	    std::function<std::chrono::system_clock::duration(void)> callable,
	    std::chrono::system_clock::duration minInterval =
		std::chrono::system_clock::duration(std::chrono::seconds(1)));
	~NicoLiveTimer();
	void Start();
	void Stop();
	bool IsActive() const;

private:
	static void Loop(int id, NicoLiveTimer *timer);
};
