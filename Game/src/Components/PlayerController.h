#pragma once
#include "Chaos/Entity/Component.h"
#include "Chaos/DataTypes/Vec2.h"

class PlayerController : public Chaos::Component
{
public:
	void Update() override;

	Chaos::Vec2 GetForward() { return m_forward; }

private:
	Chaos::Vec2 m_forward = Chaos::Vec2(0.0f, 1.0f);
	Chaos::Vec2 m_direction = Chaos::Vec2::Zero();
	Chaos::Vec2 m_velocity = Chaos::Vec2::Zero();
	float m_moveSpeed = 10.0f;
};