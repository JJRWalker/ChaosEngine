#pragma once
#include "Chaos/Entity/Component.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Components/PlayerAnimatorController.h"

namespace Chaos
{
	class PlayerController : public Chaos::Component
	{
		public:
		PlayerController() {};
		void Start() override;
		void Update() override;
		
		Vec2 GetForward() { return m_forward; }
		
		private:
		PlayerAnimatorController* m_animator;
		
		Vec2 m_forward = Vec2(0.0f, 1.0f);
		Vec2 m_direction = Vec2::Zero();
		Vec2 m_velocity = Vec2::Zero();
		float m_moveSpeed = 5.0f;
	};
}