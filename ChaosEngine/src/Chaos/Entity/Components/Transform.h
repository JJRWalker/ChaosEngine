#pragma once
#include "chaospch.h"
#include "Chaos/Entity/Component.h"
#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class Transform : public Component
	{
	public:
		Transform() {  };
		Vec2 Position;
		Vec2 Rotation;
		Vec2 Scale;

		// Inherited via Component
		virtual void Start() const override;
		virtual void Update() const override;
		virtual void Destroy() const override;
	};
}