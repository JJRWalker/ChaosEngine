#pragma once

//Stores all time based variables so they don't have to be explicitly passed to layers / can be gotten from anywhere in the applciation
//updated in Application.cpp
namespace Chaos
{
	class Time
	{
		friend class Application;
		public:
		//inits all resources, must be done before the first frame, default timescale is 1
		//TODO: load from project file
		static void Init() {
			m_deltaTime = 0;
			m_timeScale = 1.0f;
			m_time = 0.0f;
			m_timeLastFrame = 0.0f;
			m_fixedDeltaTime = 0.016f;
			m_timeSinceLastFixedUpdate = 0.0f;
		}
		
		inline static float GetDeltaTime() { return m_deltaTime * m_timeScale; }
		inline static float GetUnscaledDeltaTime() { return m_deltaTime; }
		inline static float GetFixedDeltaTime() { return m_fixedDeltaTime * m_timeScale; }
		inline static float GetUnscaledFixedDeltaTime() { return m_fixedDeltaTime; }
		
		inline static void SetTimeScale(float value) { m_timeScale = value; }
		inline static float GetTimeScale() { return m_timeScale; }
		inline static void SetFixedDeltaTimestep(float value) { m_fixedDeltaTime = value; }
		private:
		static float m_deltaTime;
		static float m_fixedDeltaTime;
		static float m_timeSinceLastFixedUpdate;
		static float m_timeScale;
		static double m_time;
		static double m_timeLastFrame;
	};
}