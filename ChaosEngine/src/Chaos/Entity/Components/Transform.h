
#pragma once
#include "chaospch.h"
#include "Chaos/Entity/Component.h"
#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class Transform : public Component
	{
	public:
		Transform() { };
		Vec2& Position() { return m_position; }
		Vec2& Rotation() { return m_rotation; }
		Vec2& Scale() { return m_scale; }

		// Inherited via Component

	private:

		Vec2 m_position = Vec2::Zero();
		Vec2 m_rotation = Vec2::Zero();
		Vec2 m_scale = Vec2(1.0f, 1.0f);
		int x = 3;
		float y = 10;
	};
}
