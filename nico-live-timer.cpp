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
      alive(std::make_shared<bool>(true))
{
}

NicoLiveTimer::~NicoLiveTimer()
{
	Stop();
	if (this->loopThread.joinable()) this->loopThread.detach();
	*this->alive = false;
	this->alive.reset();
}
void NicoLiveTimer::Start()
{
	std::lock_guard<std::mutex> lock(this->loopMutex);
	if (IsActive()) return;
	this->loopId++;
	int currentLoopId = this->loopId;
	this->active = true;
	if (this->loopThread.joinable()) this->loopThread.detach();
	this->loopThread = std::thread(Loop, currentLoopId, this);
}
void NicoLiveTimer::Stop()
{
	std::lock_guard<std::mutex> lock(this->loopMutex);
	this->active.store(false);
}
bool NicoLiveTimer::IsActive() const { return this->active; }

void NicoLiveTimer::Loop(int id, NicoLiveTimer *timer)
{
	std::shared_ptr<bool> timerAlive = timer->alive;
	while (*timerAlive) {
		std::lock_guard<std::mutex> lock(timer->loopMutex);
		if (!timer->active || id != timer->loopId) {
			break;
		}
		auto interval_time = timer->callable();
		std::this_thread::sleep_for(
		    std::max(timer->minInterval, interval_time));
	}
	timerAlive.reset();
}
