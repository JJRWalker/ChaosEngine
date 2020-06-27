#pragma once
#include "chaospch.h"
#include "Chaos/Entity/Component.h"
#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class Transform : public Component
	{
	public:
		Transform() {};
		Vec2& Position() { return mPosition; }
		Vec2& Rotation() { return mRotation; }
		Vec2& Scale() { return mScale; }

		// Inherited via Component
		virtual void Start() const override;
		virtual void Update() const override;
		virtual void Destroy() const override;
	private:
		Vec2 mPosition = Vec2::Zero();
		Vec2 mRotation = Vec2::Zero();
		Vec2 mScale = Vec2(1.0f, 1.0f);

	};
}