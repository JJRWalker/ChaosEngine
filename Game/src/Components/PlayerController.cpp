#include "chaospch.h"
#include "PlayerController.h"
#include "Chaos.h"
#include "Chaos/Core/Log.h"

namespace Chaos
{
	void PlayerController::Start()
	{
		m_animator = GetEntity()->GetComponent<PlayerAnimatorController>();
	}
	
	void PlayerController::Update()
	{
		if (Chaos::Input::IsKeyPressed(KEY_W))
		{
			m_direction.Y = 1;
			m_animator->SetAnimationState(AnimationState::RUNNING);
		}
		else if (Chaos::Input::IsKeyPressed(KEY_S))
		{
			m_direction.Y = -1;
			m_animator->SetAnimationState(AnimationState::RUNNING);
		}
		else
		{
			m_direction.Y = 0;
		}
		
		if (Chaos::Input::IsKeyPressed(KEY_A))
		{
			m_direction.X = -1;
			m_animator->SetAnimationState(AnimationState::RUNNING);
			GetEntity()->GetScale() = Vec2(-2, 2);
		}
		else if (Chaos::Input::IsKeyPressed(KEY_D))
		{
			m_direction.X = 1;
			m_animator->SetAnimationState(AnimationState::RUNNING);
			GetEntity()->GetScale() = Vec2(2, 2);
		}
		else
		{
			m_direction.X = 0;
		}
		
		if (m_direction.X == 0 && m_direction.Y == 0)
		{
			m_animator->SetAnimationState(AnimationState::IDLE);
		}
		
		
		
		//m_direction = Chaos::Vec2( m_direction.Y * m_forward.X, m_direction.Y * m_forward.Y);
		m_velocity = m_direction * m_moveSpeed * Chaos::Time::GetDeltaTime();
		GetEntity()->GetPosition() += Chaos::Vec3(m_velocity.X, m_velocity.Y, 0);
		
		
		
		//if (!Chaos::Ray2D::Cast(Chaos::Vec2(GetEntity()->GetTransform()->Position().X, GetEntity()->GetTransform()->Position().Y), m_direction, 1.0f))
		//{
	}
}