#include "chaospch.h"
#include "Ray2D.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Core/SceneManager.h"
#include "Chaos/Entity/Components/BoxCollider2D.h"

namespace Chaos
{
	RayHit2DInfo* Ray2D::Cast(Vec2& origin, Vec2& direction, float distance)
	{
		Scene* scene = SceneManager::GetScene();
		RayHit2DInfo* hitInfo = nullptr;

		Vec2 ray = direction * distance;

		Vec2 endPoint = origin + ray;
		Vec2 closestPoint = endPoint;	//init the closest point to the end point of the ray.
		
		float shortestDistance = distance;

		Collider* hit = nullptr;

		for (auto* entity : scene->GetEntities())
		{
			//test line collision with all colliders
			if (entity->HasComponent<Collider>())
			{
				//check for collision, handle collision math here as we need to return more info than just a bool if it hit or not
				Collider* collider = entity->GetComponent<Collider>();
				switch (collider->GetType())
				{
				case ColliderType::BOX2D:
				{
					float fLow = 0.0f;
					float fHigh = 1.0f;

					float boxLeft =  collider->GetEntity()->GetTransform()->Position().X - (((BoxCollider2D*)collider)->GetBounds().Width / 2);
					float boxRight =  collider->GetEntity()->GetTransform()->Position().X + (((BoxCollider2D*)collider)->GetBounds().Width / 2);
					float boxBottom =  collider->GetEntity()->GetTransform()->Position().Y - (((BoxCollider2D*)collider)->GetBounds().Height / 2);
					float boxTop =  collider->GetEntity()->GetTransform()->Position().Y + (((BoxCollider2D*)collider)->GetBounds().Height / 2);
			
					
					if (!ClipLine(boxLeft, boxRight, origin.X, endPoint.X, fLow, fHigh))
						continue;

					if(!ClipLine(boxBottom, boxTop, origin.Y, endPoint.Y, fLow, fHigh))
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
				//releasing resources
				collider = nullptr;
				delete collider;
			}
		}

		if (hit)
		{
			//if uninitialised, init the hit info
			if (!hitInfo)
				hitInfo = new RayHit2DInfo();

			hitInfo->Collider = hit;
			hitInfo->Distance = shortestDistance;
			hitInfo->Point = closestPoint;
			hitInfo->Entity = hit->GetEntity();
			hitInfo->Transform = hitInfo->Entity->GetTransform();
		}

		return hitInfo;
	}
	void Ray2D::DrawRay(Vec2& origin, Vec2& direction, float distance)
	{
		Application::Get().GetRenderer().DrawQuad(Vec3(origin.X, origin.Y, 100.0f), Vec2(0.01f, 0.01f), Texture::GetBlank());
		Application::Get().GetRenderer().DrawQuad(Vec3(origin.X + (direction.X * distance), origin.Y + (direction.Y * distance), 100), Vec2(0.01f, 0.01f), Texture::GetBlank());
		Application::Get().GetRenderer().DrawLine(origin, origin + (direction * distance), Vec4(1, 0, 0, 1), 0.01f, 99);
	}
	bool Ray2D::ClipLine(float minBounds, float maxBounds, float origin, float end, float& fLow, float& fHigh)
	{
		float newLow, newHigh;

		newLow = (minBounds - origin) / (end - origin);
		newHigh = (maxBounds - origin) / (end - origin);

		if (newLow > newHigh)
		{
			float temp = newHigh;
			newHigh = newLow;
			newLow = temp;
		}

		if (newHigh < fLow)
			return false;
		
		if (newLow > fHigh)
			return false;

		fLow = std::max(newLow, fLow);
		fHigh = std::min(newHigh, fHigh);

		if (fLow > fHigh)
			return false;

		return true;
	}
}