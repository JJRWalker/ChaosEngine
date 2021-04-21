/* date = February 3rd 2021 7:29 am */

#ifndef _COLLISIONS_H
#define _COLLISIONS_H

#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class Collisions
	{
		public:
		static bool BoxBoxIntersection(Vec2 aOrigin, Vec2 aBounds, Vec2 bOrigin, Vec2 bBounds);
		static bool CircleCircleIntersection(Vec2 aOrigin, float aRadius, Vec2 bOrigin, float bRadius);
		static bool LineLineIntersection(Vec2  aStart, Vec2 aEnd, Vec2 bStart, Vec2 bEnd);
		static bool LineBoxIntersection(Vec2 lineStart, Vec2 lineEnd, Vec2 boxOrigin, Vec2 boxBounds);
		static bool LineCircleIntersection(Vec2 lineStart, Vec2 lineEnd, Vec2 circleOrigin, float radius);
		static bool CircleBoxIntersection(Vec2 circleOrigin, float radius, Vec2 boxOrigin, Vec2 boxBounds);
		
		static bool PointInRectangle(Vec2 point, Vec2 rectOrigin, Vec2 rectBounds);
		static bool PointInCircle(Vec2 point, Vec2 circleOrigin, float radius);
	};
}

#endif //_COLLISIONS_H
