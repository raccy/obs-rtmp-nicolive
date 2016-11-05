#pragma once

#include <atomic>
#include <ctime>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

class NicoLiveTimer
{
	std::atomic_llong intervalMsec;
	const std::function<std::time_t(std::time_t)> callable;

	std::atomic_bool active;
	std::atomic_int loopId;
	std::thread loopThread;
	std::mutex loopMutex;

	std::shared_ptr<bool> alive;

public:
	NicoLiveTimer() = delete;
	NicoLiveTimer(long long intervalMsec,
	    std::function<std::time_t(std::time_t)> callable);
	~NicoLiveTimer();
	void Start();
	void Stop();
	bool IsActive() const;
	void SetIntevalMsec(long long intervalMsec);

private:
	static void Loop(int id, NicoLiveTimer *timer);
};
