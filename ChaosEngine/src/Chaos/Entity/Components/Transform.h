#pragma once
#include "chaospch.h"
#include "Chaos/Entity/Component.h"
#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class Transform : public IComponent
	{
	public:
		Transform() {};
		Vec2& Position() { return m_position; }
		Vec2& Rotation() { return m_rotation; }
		Vec2& Scale() { return m_scale; }

		// Inherited via Component
		virtual void Start() override;
		virtual void Update() override;
		virtual void Destroy() override;
	private:
		Vec2 m_position = Vec2::Zero();
		Vec2 m_rotation = Vec2::Zero();
		Vec2 m_scale = Vec2(1.0f, 1.0f);

	};
}