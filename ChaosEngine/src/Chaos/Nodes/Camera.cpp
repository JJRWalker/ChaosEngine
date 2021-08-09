#include "chaospch.h"

#include "Camera.h"

#include "Chaos/Core/Application.h"
#include "Chaos/Core/Level.h"
#include <GLM/glm/gtc/matrix_transform.hpp>

#include "Chaos/Debug/ImGuiLayer.h"

namespace Chaos
{
	Camera::Camera()
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
	
	
	Binary Camera::SaveToBinary()
	{
		Binary baseSpriteBinary = Node::SaveToBinary();
		
		// capacity should also be the number of bytes in the binary
		size_t finalDataSize = baseSpriteBinary.Capacity();
		
		Binary bounds((void*)&m_bounds.X, sizeof(float) * 4);
		finalDataSize += sizeof(float) * 4;
		
		Binary aspect((void*)&m_aspectRatio, sizeof(float));
		finalDataSize += sizeof(float);
		
		Binary projectionType((void*)&m_projectionType, sizeof(EProjectionType));
		finalDataSize += sizeof(EProjectionType);
		
		Binary backgroundColour((void*)&BackgroundColour, sizeof(float) * 4);
		finalDataSize += sizeof(float) * 4;
		
		Binary data(finalDataSize);
		data.Write(baseSpriteBinary.Data, baseSpriteBinary.Capacity());
		data.Write(bounds.Data, bounds.Capacity());
		data.Write(aspect.Data, aspect.Capacity());
		data.Write(projectionType.Data, projectionType.Capacity());
		data.Write(backgroundColour.Data, backgroundColour.Capacity());
		
		return data;
	}
	
	
	size_t Camera::LoadFromBinary(char* data)
	{
		size_t location = Node::LoadFromBinary(data);
		
		switch (m_nodeVersion)
		{
			case NODE_VERSION_0:
			{
				memcpy((void*)&m_bounds, (void*)&data[location], sizeof(float) * 4);
				location += sizeof(float) * 4;
				
				memcpy((void*)&m_aspectRatio, (void*)&data[location], sizeof(float));
				location += sizeof(float);
				
				memcpy((void*)&m_projectionType, (void*)&data[location], sizeof(EProjectionType));
				location += sizeof(EProjectionType);
				
				memcpy((void*)&BackgroundColour, (void*)&data[location], sizeof(float) * 4);
				location += sizeof(float) * 4;
				
				return location;
				
			} break;
		}
		
		return location;
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