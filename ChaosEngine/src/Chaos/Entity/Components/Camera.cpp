#include "chaospch.h"

#include "Camera.h"
#include "Chaos/Core/Application.h"

#include <GLM/glm/gtc/matrix_transform.hpp>

namespace Chaos
{
	void Camera::Recalculate()
	{
		m_view = glm::lookAt(glm::vec3(m_entity->GetTransform()->Position().X, m_entity->GetTransform()->Position().Y, 1), glm::vec3(m_entity->GetTransform()->Position().X, m_entity->GetTransform()->Position().Y, -1), glm::vec3(0.0f, 1.0f, 0.0f));
		m_model = glm::rotate(glm::mat4(1.0f), glm::radians(m_entity->GetTransform()->Rotation().X), glm::vec3(0.0f, 0.0f, 1.0f));
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
		if (m_entity)
		{
			Recalculate();
		}
	}
	void Camera::Destroy() 
	{
	}
}