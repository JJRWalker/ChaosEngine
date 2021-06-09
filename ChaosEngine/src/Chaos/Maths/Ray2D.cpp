#include "chaospch.h"

#include "Ray2D.h"
#include "Chaos/Core/Level.h"
#include "Chaos/Nodes/Colliders.h"
#include "Chaos/DataTypes/QuadTree.h"

#include "Chaos/Debug/Debug.h"

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
		
		QuadTree quadTree;
		
		for (int node = 0; node < level->NodeCount; ++node)
		{
			for (int child = 0; child <= level->Nodes[node][0]->ChildCount; ++child)
			{
				if (level->Nodes[node][child]->IsEnabled())
				{
					if(Collider* collider = dynamic_cast<Collider*>(level->Nodes[node][child]))
						quadTree.Insert(collider);
				}
			}
		}
		
		Collider** nodesInRange = (Collider**)malloc(quadTree.Size() * sizeof(Collider*));
		size_t nodesInRangeSize = 0;
		
		quadTree.QueryLine(origin, endPoint, nodesInRange, nodesInRangeSize);
		
		for (size_t node = 0; node < nodesInRangeSize; ++node)
		{
			Vec2 colliderPosition = nodesInRange[node]->GetPosition();
			
			switch (nodesInRange[node]->Type)
			{
				case ColliderType::BOX2D:
				{
					BoxCollider2D* boxCollider = (BoxCollider2D*)nodesInRange[node];
					
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
						hit = nodesInRange[node];
					}
					
				}break;
				
				case ColliderType::CIRCLE:
				{
					
					
				}break;
			}
		}
		
		free(nodesInRange);
		
		if (hit)
		{
			outHitInfo.Hit = true;
			outHitInfo.HitCollider = hit;
			outHitInfo.Distance = shortestDistance;
			outHitInfo.Point = closestPoint;
			
			// normal calculated differently per type
			switch(hit->Type)
			{
				case ColliderType::BOX2D:
				{
					Vec2 colliderPosition = hit->GetPosition();
					BoxCollider2D* boxCollider = (BoxCollider2D*)hit;
					
					Vec2 point = closestPoint - hit->GetPosition();
					point = point / boxCollider->Bounds;
					
					
					Vec2 topLeft = Vec2(colliderPosition.X - boxCollider->Bounds.X, colliderPosition.Y + boxCollider->Bounds.Y);
					Vec2 topRight = Vec2(colliderPosition.X + boxCollider->Bounds.X, colliderPosition.Y + boxCollider->Bounds.Y);
					Vec2 bottomLeft = Vec2(colliderPosition.X - boxCollider->Bounds.X, colliderPosition.Y - boxCollider->Bounds.Y);
					Vec2 bottomRight = Vec2(colliderPosition.X + boxCollider->Bounds.X, colliderPosition.Y - boxCollider->Bounds.Y);
					
					// not personally a fan of this part at. Seems there could be a way to simplfiy / not use branching satements
					// ray hit left or right side
					if (abs(point.X) > abs(point.Y))
					{
						// left
						if (point.X < 0)
						{
							outHitInfo.Normal = (bottomLeft - topLeft).Cross().Normalised();
						}
						// right
						else
						{
							outHitInfo.Normal = (topRight - bottomRight).Cross().Normalised();
						}
					}
					// ray hit top or bottom side
					else
					{
						// bottom
						if (point.Y < 0)
						{
							outHitInfo.Normal = (bottomRight - bottomLeft).Cross().Normalised();
						}
						// top
						else
						{
							outHitInfo.Normal = (topLeft - topRight).Cross().Normalised();
						}
					}
				}break;
				
				default:
				{
					// this should work for circle colliders
					outHitInfo.Normal = (closestPoint - origin).Normalised();
				}break;
			}
			
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