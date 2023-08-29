#include "Timer.h"
#include "Clock.h"

#include <CommonWindows.h>

#if defined(DEATH_TARGET_WINDOWS)
#	include <synchapi.h>
#else
#	include <unistd.h>
#	if defined(DEATH_TARGET_SWITCH)
#		include <switch.h>
#	endif
#endif

namespace nCine
{
	Timer::Timer()
		: isRunning_(false), startTime_(clock().now()), accumulatedTime_(0ULL)
	{
	}

	void Timer::start()
	{
		isRunning_ = true;
		startTime_ = clock().counter();
	}

	void Timer::stop()
	{
		accumulatedTime_ += clock().counter() - startTime_;
		isRunning_ = false;
	}

	float Timer::interval() const
	{
		return static_cast<float>(clock().counter() - startTime_) / clock().frequency();
	}

	float Timer::total() const
	{
		return isRunning_
			? static_cast<float>(accumulatedTime_ + clock().counter() - startTime_) / clock().frequency()
			: static_cast<float>(accumulatedTime_) / clock().frequency();
	}

	void Timer::sleep(float seconds)
	{
#if defined(DEATH_TARGET_SWITCH)
		const std::int64_t nanoseconds = static_cast<std::int64_t>(seconds) * 1000000000;
		svcSleepThread(nanoseconds);
#elif defined(DEATH_TARGET_WINDOWS)
		const unsigned int milliseconds = static_cast<unsigned int>(seconds) * 1000;
		::SleepEx(milliseconds, FALSE);
#else
		const unsigned int microseconds = static_cast<unsigned int>(seconds) * 1000000;
		::usleep(microseconds);
#endif
	}
}
