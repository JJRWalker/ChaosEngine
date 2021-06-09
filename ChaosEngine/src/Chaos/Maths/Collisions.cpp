#include "chaospch.h"

#include "Collisions.h"

namespace Chaos
{
	bool Collisions::BoxBoxIntersection(Vec2 aOrigin, Vec2 aBounds, Vec2 bOrigin, Vec2 bBounds)
	{
		Vec2 aMin = Vec2(aOrigin.X - aBounds.X, aOrigin.Y - aBounds.Y);
		Vec2 aMax = Vec2(aOrigin.X + aBounds.X, aOrigin.Y + aBounds.Y);
		Vec2 bMin = Vec2(bOrigin.X - bBounds.X, bOrigin.Y - bBounds.Y);
		Vec2 bMax = Vec2(bOrigin.X + bBounds.X, bOrigin.Y + bBounds.Y);
		
		return !(aMax.X < bMin.X || aMin.X > bMax.X || aMin.Y > bMax.Y || aMax.Y < bMin.Y);
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
		Vec2 topLeft = Vec2(boxOrigin.X - boxBounds.X, boxOrigin.Y + boxBounds.Y);
		Vec2 topRight = Vec2(boxOrigin.X + boxBounds.X, boxOrigin.Y + boxBounds.Y);
		Vec2 bottomLeft = Vec2(boxOrigin.X - boxBounds.X, boxOrigin.Y - boxBounds.Y);
		Vec2 bottomRight = Vec2(boxOrigin.X + boxBounds.X, boxOrigin.Y - boxBounds.Y);
		
		float lineCross = lineEnd.X * lineStart.Y - lineStart.X * lineEnd.Y;
		float yDist = lineEnd.Y - lineStart.Y;
		float xDist = lineStart.X - lineEnd.X;
		
		float topLeftSide = yDist * topLeft.X + xDist * topLeft.Y + lineCross;
		float topRightSide = yDist * topRight.X + xDist * topRight.Y + lineCross;
		float bottomLeftSide = yDist * bottomLeft.X + xDist * bottomLeft.Y + lineCross;
		float bottomRightSide = yDist * bottomRight.X + xDist * bottomRight.Y + lineCross;
		
		if ((topLeftSide < 0 && topRightSide < 0 && bottomLeftSide < 0 && bottomRightSide < 0) 
			||(topLeftSide > 0 && topRightSide > 0 && bottomLeftSide > 0 && bottomRightSide > 0))
			return false;
		
		
		return true;
	}
	
	
	bool Collisions::LineCircleIntersection(Vec2 lineStart, Vec2 lineEnd, Vec2 circleOrigin, float radius)
	{
		Vec2 lineVec = (lineEnd - lineStart);
		Vec2 basisVector = (lineEnd - lineStart).Normalised();
		
		Vec2 lineStartToCircle = circleOrigin - lineStart;
		
		float linePercent = (lineStartToCircle / basisVector).Magnitude() / lineVec.Magnitude();
		
		if (linePercent < 0.f || linePercent > 1.0f)
			return false;
		
		Vec2 projectedPoint = lineStart + lineVec * linePercent;
		
		return (circleOrigin - projectedPoint).Magnitude() <= radius;
	}
	
	
	bool Collisions::CircleBoxIntersection(Vec2 circleOrigin, float radius, Vec2 boxOrigin, Vec2 boxBounds)
	{
		// for reference, came from an answer to this question: https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection
		// gets the closest point inside the box and then checks if it's further away than the radius
		float closestX = std::clamp(circleOrigin.X, boxOrigin.X - boxBounds.X, boxOrigin.X + boxBounds.X);
		float closestY = std::clamp(circleOrigin.Y, boxOrigin.Y - boxBounds.Y, boxOrigin.Y + boxBounds.Y);
		
		Vec2 distance = Vec2(circleOrigin.X - closestX, circleOrigin.Y - closestY);
		
		float distSq = (distance.X * distance.X) + (distance.Y * distance.Y);
		
		return distSq < (radius* radius);
	}
	
	
	bool Collisions::PointInRectangle(Vec2 point, Vec2 rectOrigin, Vec2 rectBounds)
	{
		float halfWidth = abs(rectBounds.X);
		float halfHeight = abs(rectBounds.Y);
		
		return (point.X >= rectOrigin.X - halfWidth && point.X <= rectOrigin.X + halfWidth) 
			&& (point.Y >= rectOrigin.Y - halfHeight && point.Y <= rectOrigin.Y + halfHeight);
	}
	
	
	bool Collisions::PointInCircle(Vec2 point, Vec2 circleOrigin, float radius)
	{
		float distance = (point - circleOrigin).Magnitude();
		return distance < radius;
	}
}