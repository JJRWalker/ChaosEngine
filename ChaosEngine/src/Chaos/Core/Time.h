#pragma once

#include <ctime>
#include <chrono>

namespace Chaos
{
	class Time
	{
	public:
		float DeltaTime();

		void StartDeltaTiming();
		void EndDeltaTiming();

	private:
		float mDeltaTime;
	};
}