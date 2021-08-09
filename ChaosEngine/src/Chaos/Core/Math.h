#pragma once

//stores constants for maths and other basic mathmatic functions
namespace Chaos
{
#define PI 3.14159265358979323846f
#define HALF_PI 1.57079632679489661923f
	
	class Math
	{
		public:
		//these can just be public because they're unmutable therefore cannot be changed outside of this delaration
		//static inline const float PI = 3.14159265358979323846f;
		static inline const float RAD_TO_DEGREES = 180 / PI;
		static inline const float DEGREES_TO_RADS = PI / 180;
		
		static inline float Lerp(float a, float b, float t)
		{
			return a + t * (b - a);
		}
	};
}