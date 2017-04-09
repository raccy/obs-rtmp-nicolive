#include "nico-live-timer.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

NicoLiveTimer::NicoLiveTimer(
    std::function<std::chrono::milliseconds(void)> callable,
    std::chrono::milliseconds minInterval)
    : callable(callable), minInterval(minInterval), active(false), loopId(0),
      loopMutex(std::make_shared<std::mutex>()),
      alive(std::make_shared<std::atomic_bool>(true))
{
}

NicoLiveTimer::~NicoLiveTimer()
{
	Stop();
	*this->alive = false;
}
void NicoLiveTimer::Start()
{
	std::lock_guard<std::mutex> lock(*this->loopMutex);
	if (IsActive()) return;
	this->loopId++;
	int currentLoopId = this->loopId;
	this->active = true;
	std::thread th(Loop, currentLoopId, this);
	th.detach();
}
void NicoLiveTimer::Stop()
{
	std::lock_guard<std::mutex> lock(*this->loopMutex);
	this->active.store(false);
}
bool NicoLiveTimer::IsActive() const { return this->active; }

void NicoLiveTimer::Loop(int id, NicoLiveTimer *timer)
{
	auto timerAlive = timer->alive;
	auto timerLoopMutex = timer->loopMutex;
	auto intervalTime = timer->minInterval;
	while (true) {
		{
			std::lock_guard<std::mutex> lock(*timerLoopMutex);
			if (!*timerAlive) break;
			if (!timer->active || id != timer->loopId) break;
			intervalTime = timer->callable();
		}
		std::this_thread::sleep_for(
		    std::max(timer->minInterval, intervalTime));
	}
}
