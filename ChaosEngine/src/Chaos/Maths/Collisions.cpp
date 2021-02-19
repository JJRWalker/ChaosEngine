#include "chaospch.h"

#include "Collisions.h"

namespace Chaos
{
	bool Collisions::BoxBoxIntersection(Vec2 aOrigin, Vec2 aBounds, Vec2 bOrigin, Vec2 bBounds)
	{
		return !(bOrigin.X - bBounds.X * 0.5f > aOrigin.X + aBounds.X * 0.5f || 
			bOrigin.X + bBounds.X * 0.5f < aOrigin.X - aBounds.X * 0.5f || 
			bOrigin.Y - bBounds.Y * 0.5f > aOrigin.Y + aBounds.Y * 0.5f || 
			bOrigin.Y + bBounds.Y * 0.5f < aOrigin.Y - aBounds.Y * 0.5f); 
	}
	
	
	bool Collisions::CircleCircleIntersection(Vec2 aOrigin, float aRadius, Vec2 bOrigin, float bRadius)
	{
		return aRadius + bRadius < (bOrigin - aOrigin).Magnitude();
	}
	
	
	bool Collisions::LineLineIntersection(Vec2  aStart, Vec2 aEnd, Vec2 bStart, Vec2 bEnd)
	{
		LOGCORE_WARN("WARN: Collisions.cpp LineLineIntersection is not implemented yet!");
		return false;
	}
	
	
	bool Collisions::LineBoxIntersection(Vec2 lineStart, Vec2 lineEnd, Vec2 boxOrigin, Vec2 boxBounds)
	{
		LOGCORE_WARN("WARN: Collisions.cpp LineBoxIntersection is not implemented yet!");
		return false;
	}
	
	
	bool Collisions::LineCircleIntersection(Vec2 lineStart, Vec2 lineEnd, Vec2 circleOrigin, float radius)
	{
		LOGCORE_WARN("WARN: Collisions.cpp LineCircleIntersection is not implemented yet!");
		return false;
	}
	
	
	bool Collisions::CircleBoxIntersection(Vec2 circleOrigin, float radius, Vec2 boxOrigin, Vec2 boxBounds)
	{
		// for reference, came from an answer to this question: https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection
		float closestX = std::clamp(circleOrigin.X, boxOrigin.X - boxBounds.X * 0.5f, boxOrigin.X + boxBounds.X * 0.5f);
		float closestY = std::clamp(circleOrigin.Y, boxOrigin.Y - boxBounds.Y * 0.5f, boxOrigin.Y + boxBounds.Y * 0.5f);

		Vec2 distance = boxOrigin - circleOrigin;

		float distSq = (distance.X * distance.X) + (distance.Y * distance.Y);

		return distSq < (radius* radius);
	}
	
	
	bool Collisions::PointInRectangle(Vec2 point, Vec2 rectOrigin, Vec2 rectBounds)
	{
		float halfWidth = rectBounds.X * 0.5f;
		float halfHeight = rectBounds.Y * 0.5f;
		
		return point.X >= rectOrigin.X - halfWidth &&
			point.X <= rectOrigin.X + halfWidth &&
			point.Y >= rectOrigin.Y - halfHeight &&
			point.Y <= rectOrigin.Y + halfHeight;
	}
	
	
	bool Collisions::PointInCircle(Vec2 point, Vec2 circleOrigin, float radius)
	{
		float distance = (point - circleOrigin).Magnitude();
		return distance < radius;
	}
}