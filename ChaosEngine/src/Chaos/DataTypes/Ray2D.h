#pragma once
#include "Vec2.h"
#include "Chaos/Entity/Components/Collider.h"
#include "Chaos/Entity/Components/Transform.h"

namespace Chaos
{
	struct RayHit2DInfo
	{
		Vec2 Point;
		Vec2 Normal;
		float Distance;
		Collider* Collider;
		Transform* Transform;
		Entity* Entity;
	};
	class Ray2D
	{
	public:
		//Returns null if not hit, else returns a ray hit struct pointer
		static RayHit2DInfo* Cast(Vec2& origin, Vec2& direction, float distance);
		//Renders a line that represents the ray, used for debugging, if you want to just draw a line, use DrawLine on the Renderer class instead
		static void DrawRay(Vec2& origin, Vec2& direction, float distance);
	private:
		static bool ClipLine(float minBounds, float maxBounds, float origin, float end, float& fLow, float& fHigh);
	};
}