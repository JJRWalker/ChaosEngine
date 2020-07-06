#include "chaospch.h"
#include "Transform.h"

void Chaos::Transform::Start() 
{
}

void Chaos::Transform::Update() 
{
	LOGCORE_INFO("x: {0} y: {1}", m_position.X, m_position.Y);
}

void Chaos::Transform::Destroy() 
{
}
