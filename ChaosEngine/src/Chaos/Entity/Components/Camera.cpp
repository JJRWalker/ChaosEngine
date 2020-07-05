#include "chaospch.h"

#include "Camera.h"
#include "Chaos/Core/Application.h"

#include <GLM/glm/gtc/matrix_transform.hpp>

namespace Chaos
{
	void Camera::Recalculate()
	{
		m_view = glm::lookAt(glm::vec3(m_position.X, m_position.Y, m_position.Z), glm::vec3(m_position.X, m_position.Y, -1), glm::vec3(0.0f, 1.0f, 0.0f));


		m_projection = glm::ortho(m_bounds.X * (float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight(),
			m_bounds.Y * (float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight(),
			m_bounds.Z, m_bounds.W, -5.0f, 5.0f);
		m_projection[1][1] *= -1;
	}
}