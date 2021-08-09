/* date = May 30th 2021 1:15 pm */

#ifndef _RAY2_D_H
#define _RAY2_D_H

#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class Collider;
	
	struct Ray2DHitInfo
	{
		bool Hit = false;
		Vec2 Point;
		Vec2 Normal;
		float Distance;
		Collider* HitCollider;
	};
	
	class Ray2D
	{
		public:
		static bool Cast(Vec2& origin, Vec2& direction, float distance);
		static bool Cast(Vec2& origin, Vec2& direction, float distance, Ray2DHitInfo& outHitInfo);
		// if the value '0' is passed in as a mask, the mask will be ignored
		static bool Cast(Vec2& origin, Vec2& direction, float distance, Ray2DHitInfo& outHitInfo, uint32_t mask);
		
		private: 
		static bool ClipLine(float minBounds, float maxBounds, float origin, float end, float& fLow, float& fHigh);
		
	};
}

#endif //_RAY2_D_H
