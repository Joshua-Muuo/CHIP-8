#pragma once
#include <chrono>
#include <format>
#include "types.hpp"

class Clock
{
	using ClockType = std::chrono::steady_clock;
	using milliseconds = std::chrono::duration<f64, std::milli>;
	public:
		Clock(f64 timePerTick):
		mLast{ClockType::now()},
		mDeltaTime{0.0},
		mTimePerTick{timePerTick}
		{};

		[[nodiscard]] i32 tick() 
		{ 
			namespace chr = std::chrono;
			using namespace std::chrono_literals;
			const auto now{chr::steady_clock::now()};
			const auto elapsedTime{chr::duration_cast<milliseconds>(now - mLast)};
			mDeltaTime += elapsedTime;
			
			bool timerDone{mDeltaTime >= mTimePerTick};
			i32 ticks{};
			if (timerDone)
			{
				ticks = mDeltaTime / mTimePerTick;
				mDeltaTime -= ticks * mTimePerTick;
			}

			mLast = now;
			return ticks;
		}

		
	private:
		ClockType::time_point mLast;
		milliseconds mDeltaTime, mTimePerTick;
};
