#include "chaospch.h"

#include "Ray2D.h"
#include "Chaos/Core/Level.h"
#include "Chaos/Nodes/Colliders.h"

namespace Chaos
{
	bool Ray2D::Cast(Vec2& origin, Vec2& direction, float distance)
	{
		Ray2DHitInfo hitInfo;
		return Cast(origin, direction, distance, hitInfo);
	}
	
	
	bool Ray2D::Cast(Vec2& origin, Vec2& direction, float distance, Ray2DHitInfo& outHitInfo)
	{
		Level* level = Level::Get();
		
		Vec2 ray = direction.Normalised() * distance;
		
		Vec2 endPoint = origin + ray;
		Vec2 closestPoint = endPoint;
		
		float shortestDistance = distance;
		
		Collider* hit = nullptr;
		
		for (size_t node = 0; node < level->NodeCount; ++node)
		{
			for (size_t child = 0; child <= level->Nodes[node][0]->ChildCount; ++child)
			{
				if (Collider* collider = dynamic_cast<Collider*>(level->Nodes[node][child]))
				{
					Vec2 colliderPosition = collider->GetPosition();
					
					switch (collider->Type)
					{
						case ColliderType::BOX2D:
						{
							BoxCollider2D* boxCollider = (BoxCollider2D*)collider;
							
							float boxLeft = colliderPosition.X - boxCollider->Bounds.X;
							float boxRight = colliderPosition.X + boxCollider->Bounds.X;
							float boxBottom = colliderPosition.Y - boxCollider->Bounds.Y;
							float boxTop = colliderPosition.Y + boxCollider->Bounds.Y;
							
							float fLow = 0.0f;
							float fHigh = 1.0f;
							
							if (!ClipLine(boxLeft, boxRight, origin.X, endPoint.X, fLow, fHigh))
								continue;
							
							if (!ClipLine(boxBottom, boxTop, origin.Y, endPoint.Y, fLow, fHigh))
								continue;
							
							Vec2 pointOfIntersection = origin + (ray * fLow);
							
							if ((pointOfIntersection - origin).Magnitude() < shortestDistance)
							{
								closestPoint = pointOfIntersection;
								shortestDistance = (pointOfIntersection - origin).Magnitude();
								hit = collider;
							}
							
						}break;
					}
				}
			}
		}
		
		if (hit)
		{
			outHitInfo.HitCollider = hit;
			outHitInfo.Distance = shortestDistance;
			outHitInfo.Point = closestPoint;
			outHitInfo.Hit = true;
			return true;
		}
		outHitInfo.Hit = false;
		return false;
	}
	
	
	bool Ray2D::ClipLine(float minBounds, float maxBounds, float origin, float end, float& fLow, float& fHigh)
	{
		float newLow = (minBounds - origin) / (end - origin);
		float newHigh = (maxBounds - origin) / (end - origin);
		
		if (newLow > newHigh)
		{
			float temp = newHigh;
			newHigh = newLow;
			newLow = temp;
		}
		
		if (newHigh < fLow || newLow > fHigh)
			return false;
		
		fLow = std::max(newLow, fLow);
		fHigh = std::min(newHigh, fHigh);
		
		if (fLow > fHigh)
			return false;
		
		return true;
	}
}