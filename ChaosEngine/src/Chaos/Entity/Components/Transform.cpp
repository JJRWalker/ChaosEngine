#include "chaospch.h"
#include "Transform.h"

void Chaos::Transform::Start() const
{
}

void Chaos::Transform::Update() const
{
	LOGCORE_INFO("x: {0} y: {1}", m_position.X, m_position.Y);
}

void Chaos::Transform::Destroy() const
{
}
