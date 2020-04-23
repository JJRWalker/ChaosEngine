#pragma once

#include <ctime>
#include <chrono>

namespace Chaos
{
	class TimeStep
	{
	public:
		TimeStep(float time = 0.0f)
			: mTime(time)
		{

		}

		float DeltaTime();

		void StartDeltaTiming();
		void EndDeltaTiming();

	private:
		float mTime;
	};
}