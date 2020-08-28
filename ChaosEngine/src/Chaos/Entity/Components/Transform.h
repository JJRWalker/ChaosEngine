
#pragma once
#include "chaospch.h"
#include "Chaos/Entity/Component.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec3.h"

namespace Chaos
{
	class Transform : public Component
	{
	public:
		Transform() { };
		Vec2& Position2D() { return Vec2(m_position.X, m_position.Y); }
		Vec3& Position() { return m_position; }
		Vec2& Rotation() { return m_rotation; }
		Vec2& Scale() { return m_scale; }
		Vec2& Right() { return m_right; }
		Vec2& Up() { return m_up; }
		//TODO: rotate right and up vectors on rotation

	private:

		Vec3 m_position = Vec3::Zero();
		Vec2 m_rotation = Vec2::Zero();
		Vec2 m_scale = Vec2(1.0f, 1.0f);
		Vec2 m_right = Vec2(1.0f, 0.0f);
		Vec2 m_up = Vec2(0.0f, 0.0f);
		int x = 3;
		float y = 10;
	};
}
