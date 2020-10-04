#pragma once

//stores constants for maths and other basic mathmatic functions
namespace Chaos
{
	class Math
	{
	public:
		//these can just be public because they're unmutable therefore cannot be changed outside of this delaration
		static inline const float PI = 3.14159265358979323846f;
		static inline const float RAD_TO_DEGREES = 180 / PI;
		static inline const float DEGREES_TO_RADS = PI / 180;
	};
}