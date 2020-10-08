#include "chaospch.h"

#include "Camera.h"
#include "Chaos/Core/Application.h"

#include <GLM/glm/gtc/matrix_transform.hpp>

namespace Chaos
{
	void Camera::Recalculate()
	{
		m_view = glm::lookAt(glm::vec3(GetEntity()->GetTransform()->Position().X, GetEntity()->GetTransform()->Position().Y, 1), glm::vec3(GetEntity()->GetTransform()->Position().X, GetEntity()->GetTransform()->Position().Y, -1), glm::vec3(0.0f, 1.0f, 0.0f));
		m_model = glm::rotate(glm::mat4(1.0f), glm::radians(GetEntity()->GetTransform()->Rotation().X), glm::vec3(0.0f, 0.0f, 1.0f));
		m_projection = glm::ortho(m_bounds.X * m_aspectRatio,
								  m_bounds.Y * m_aspectRatio,
								  m_bounds.Z, m_bounds.W, -1.0f, 1.0f);
		m_projection[1][1] *= -1;
	}
	void Camera::Start() 
	{
	}
	void Camera::Update() 
	{
		if (GetEntity())
		{
			Recalculate();
		}
	}
	void Camera::Destroy() 
	{
	}
	
	Vec2 Camera::ScreenToWorld(Vec2 screenPoint)
	{
		glm::mat4 transform = m_projection * m_view * m_model;
		glm::vec2 pos = {screenPoint.X, screenPoint.Y};
		glm::vec4 multiplied = glm::inverse(transform) * glm::vec4(pos.x, pos.y, 0, 0);
		multiplied.x += GetEntity()->GetPosition().X;
		multiplied.y += GetEntity()->GetPosition().Y;
		return Vec2(multiplied.x, multiplied.y);
	}
}