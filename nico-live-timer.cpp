#include "nico-live-timer.hpp"
#include <atomic>
#include <chrono>
#include <ctime>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

NicoLiveTimer::NicoLiveTimer(
    long long intervalMsec, std::function<std::time_t(std::time_t)> callable)
    : intervalMsec(intervalMsec), callable(callable), active(false), loopId(0),
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
	this->active = false;
}
bool NicoLiveTimer::IsActive() const { return this->active; }

void NicoLiveTimer::SetIntervalMsec(long long intervalMsec)
{
	this->intervalMsec = intervalMsec;
}

void NicoLiveTimer::Loop(int id, NicoLiveTimer *timer)
{
	std::shared_ptr<bool> timerAlive = timer->alive;
	while (*timerAlive && timer->IsActive() && id == timer->loopId) {
		auto before_time = std::chrono::system_clock::now();
		auto next_intervaled_time =
		    before_time +
		    std::chrono::milliseconds(timer->intervalMsec);
		auto next_time =
		    std::chrono::system_clock::from_time_t(timer->callable(
			std::chrono::system_clock::to_time_t(before_time)));
		if (next_time > next_intervaled_time) {
			next_time = next_intervaled_time;
		}
		auto after_time = std::chrono::system_clock::now();
		if (after_time < next_time) {
			std::this_thread::sleep_for(next_time - after_time);
		}
	}
	timerAlive.reset();
}
