#include "chaospch.h"
#include "Time.h"

namespace Chaos
{
	float Time::m_deltaTime = 0.0f;
	float Time::m_fixedDeltaTime = 0.22f;
	double Time::m_time = 0.0f;
	double Time::m_timeLastFrame = 0.0f;
	float Time::m_timeScale = 1.0f;
}