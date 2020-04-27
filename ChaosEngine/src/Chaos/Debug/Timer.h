#pragma once
#include <chrono>

namespace Chaos
{
	class Timer
	{
	public:
		Timer(const char* name) : mName(name)	{		
			mStartPoint = std::chrono::high_resolution_clock::now();
		}
		~Timer() {
			if (!mStopped)
				Stop();
		}
		void Stop() {
			auto endPoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(mStartPoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();

			float duration = (end - start) * 0.001f;

			LOGCORE_TRACE("{0}: {1}ms", mName, duration);	//TO BE CHANGED TO CALLBACK FUNC TO BE TAKEN IN BY CONSTRUCTOR

			mStopped = true;
		}
	private:
		const char* mName;
		bool mStopped = false;
		std::chrono::time_point<std::chrono::steady_clock> mStartPoint;
	};
}