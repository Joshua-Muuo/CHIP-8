#pragma once
#include <chrono>
#include <format>
#include "types.hpp"

using timepoint = std::chrono::time_point<std::chrono::steady_clock>;
class Clock
{
	public:
		Clock(f64 timerValue):
		mTimer{timerValue}
		{};

		[[nodiscard]] bool tick() 
		{ 
			namespace chr = std::chrono;
			using namespace std::chrono_literals;
			mNow = chr::steady_clock::now();

			auto elapsedTime{chr::duration_cast<chr::milliseconds>(mNow - mLast)};
			mDeltaTime += elapsedTime;
			
			bool timerDone{mDeltaTime >= mTimer};
			if (timerDone)
				mDeltaTime = 0ms;

			return timerDone;
		}

		
	private:
		timepoint mLast, mNow;
		std::chrono::duration<f64, std::milli> mDeltaTime, mTimer;
};
