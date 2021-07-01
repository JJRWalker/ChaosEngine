#include "chaospch.h"

#include "Camera.h"

#include "Chaos/Core/Application.h"
#include "Chaos/Core/Level.h"
#include <GLM/glm/gtc/matrix_transform.hpp>

#include "Chaos/Debug/ImGuiLayer.h"

namespace Chaos
{
	Camera::Camera(bool child): Node(child)
	{
		Name = "Camera"; 
		Type = NodeType::CAMERA;
		Recalculate();
		
		if (!Level::Get()->MainCamera)
			Level::Get()->MainCamera = this;
	}
	
	
	Camera::~Camera()
	{
		if (Level::Get()->MainCamera == this)
			Level::Get()->MainCamera = nullptr;
	}
	
	
	void Camera::Recalculate()
	{
		switch (m_projectionType)
		{
			case EProjectionType::PERSPECTIVE:
			{
				m_view = glm::translate(glm::mat4(1.f), {-GetPosition().X, -GetPosition().Y, GetPosition3D().Z});
				
				m_projection = glm::perspective(glm::radians(70.f), m_aspectRatio, 0.1f, 200.0f);
			}break;
			case EProjectionType::ORTHAGRAPHIC:
			{
				m_view = glm::lookAt(glm::vec3(GetWorldPosition().X, GetWorldPosition().Y, 1.0f), glm::vec3(GetWorldPosition().X,  GetWorldPosition().Y, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				m_model = glm::rotate(glm::mat4(1.0f), glm::radians(GetRotation()), glm::vec3(0.0f, 0.0f, 1.0f));
				m_projection = glm::ortho(m_bounds.X * m_aspectRatio,
										  m_bounds.Y * m_aspectRatio,
										  m_bounds.Z, m_bounds.W,  -200.0f,  200.0f);
			}break;
		}
		
		m_projection[1][1] *= -1;
	}
	
	
	void Camera::OnStart() 
	{
	}
	
	
	void Camera::OnUpdate(float delta) 
	{
		
		Recalculate();
	}
	
	
	void Camera::OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle)
	{
		int selectedProjectionType = (int)m_projectionType;
		const char* projectionTypeOptions[] = { "Perspective", "Orthagraphic" };
		if (ImGui::Combo("Projection Type", &selectedProjectionType, projectionTypeOptions, IM_ARRAYSIZE(projectionTypeOptions)))
		{
			m_projectionType = (EProjectionType)selectedProjectionType;
		}
	}
	
	
	Vec2 Camera::ScreenToWorld(Vec2 screenPoint)
	{
		glm::mat4 transform = m_projection * m_view * m_model;
		glm::vec2 pos = {screenPoint.X, screenPoint.Y};
		glm::vec4 multiplied = glm::inverse(transform) * glm::vec4(pos.x, pos.y, 0, 0);
		multiplied.x += GetWorldPosition().X;
		multiplied.y += GetWorldPosition().Y;
		return Vec2(multiplied.x, multiplied.y);
	}
}