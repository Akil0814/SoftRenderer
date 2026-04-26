#pragma once

#include <chrono>

namespace mai
{

class ScopedTimer
{
public:
	explicit ScopedTimer(double& target_ms) noexcept
		: _target_ms(target_ms), _start(Clock::now())
	{
	}

	~ScopedTimer() noexcept
	{
		const auto end = Clock::now();
		_target_ms += std::chrono::duration<double, std::milli>(end - _start).count();
	}

	ScopedTimer(const ScopedTimer&) = delete;
	ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
	using Clock = std::chrono::steady_clock;

	double& _target_ms;
	Clock::time_point _start;
};

}
