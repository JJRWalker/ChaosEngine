#include "chaospch.h"
#include "Ray2D.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

namespace Chaos
{
	RayHit2DInfo* Ray2D::Cast(Vec2& origin, Vec2& direction, float distance)
	{
		return nullptr;
	}
	void Ray2D::DrawRay(Vec2& origin, Vec2& direction, float distance)
	{
		Application::Get().GetRenderer().DrawQuad(Vec3(origin.X, origin.Y, 100), Vec2(0.1, 0.1), Texture::GetBlank());
		Application::Get().GetRenderer().DrawQuad(Vec3(origin.X + (direction.X * distance), origin.Y + (direction.Y * distance), 100), Vec2(0.1, 0.1), Texture::GetBlank());
		Application::Get().GetRenderer().DrawLine(origin, origin + (direction * distance), Vec4(1, 0, 0, 1), 0.1f, 99);
	}
}