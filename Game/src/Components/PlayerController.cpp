#include "chaospch.h"
#include "PlayerController.h"
#include "Chaos.h"

void PlayerController::Update()
{
	if (Chaos::Input::IsKeyPressed(KEY_W))
	{
		m_direction.Y = 1;
	}
	else if (Chaos::Input::IsKeyPressed(KEY_S))
	{
		m_direction.Y = -1;
	}
	else
	{
		m_direction.Y = 0;
	}

	if (Chaos::Input::IsKeyPressed(KEY_A))
	{
		m_forward.Rotate(-100 * Chaos::Time::GetDeltaTime());
	}
	else if (Chaos::Input::IsKeyPressed(KEY_D))
	{
		m_forward.Rotate(100 * Chaos::Time::GetDeltaTime());
	}

	m_direction = Chaos::Vec2( m_direction.Y * m_forward.X, m_direction.Y * m_forward.Y);

	if (!Chaos::Ray2D::Cast(Chaos::Vec2(GetEntity()->GetTransform()->Position().X, GetEntity()->GetTransform()->Position().Y), m_direction, 1.0f))
	{
		m_velocity = m_direction * m_moveSpeed * Chaos::Time::GetDeltaTime();
		Chaos::Vec3 pos = GetEntity()->GetTransform()->Position() + Chaos::Vec3(m_velocity.X, m_velocity.Y, 0);
		GetEntity()->GetTransform()->Position() = pos;
	}
}