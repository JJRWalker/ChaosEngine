#pragma once

#include <ctime>
#include <chrono>

//NOT CURRENTLY USED
//will be implemented if a more advanced delta time is needed as currently it would just be a wrapper around a float
namespace Chaos
{
	class TimeStep
	{
	public:
		TimeStep(float time = 0.0f)
			: m_time(time)
		{

		}

		float DeltaTime();

		void StartDeltaTiming();
		void EndDeltaTiming();

	private:
		float m_time;
	};
}