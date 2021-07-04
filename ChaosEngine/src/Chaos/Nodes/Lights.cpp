#include "chaospch.h"

#include "Lights.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

#include "Chaos/Debug/ImGuiEditor.h"

namespace Chaos
{
	PointLight::PointLight()
	{
		Name = "Point-light";
		Type = NodeType::POINT_LIGHT_2D;
		
		p_lightObject = Application::Get().GetRenderer().AddLight(GetWorldTransform());
		
		memcpy((void*)&p_lightObject->DataArray1[0], (void*)&DEFAULT_SHADER_VALUES_1[0], sizeof(float) * 16);
	}
	
	
	PointLight::~PointLight()
	{
		Application::Get().GetRenderer().RemoveLight(p_lightObject);
		delete p_lightObject;
	}
	
	
	void PointLight::OnUpdate(float delta)
	{
		memcpy((void*)&p_lightObject->Transform[0], (void*)&GetWorldTransform()[0], sizeof(float) * 16);
	}
	
	
	void PointLight::SetEnabled(bool state)
	{
		if (Enabled == state)
			return;
		
		Enabled = state;
		if (!state)
			p_lightObject->DataArray1[15] = 0.0f;
		if (state)
			p_lightObject->DataArray1[15] = 1.0f;
	}
	
	
	void PointLight::OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle)
	{
		float colour[3] = { GetColour().X, GetColour().Y, GetColour().Z };
		float range = GetRange();
		float intensity = GetIntensity();
		
		if (ImGui::ColorEdit3("Colour", colour))
		{
			SetColour(Vec3(colour[0], colour[1], colour[2]));
		}
		if (ImGui::DragFloat("Range", &range, 0.01f))
		{
			SetRange(range);
		}
		if (ImGui::DragFloat("Intensity", &intensity, 0.01f))
		{
			SetIntensity(intensity);
		}
	}
	
	
	void PointLight::SetColour(Vec3 colour)
	{
		p_lightObject->DataArray1[0] = colour.X;
		p_lightObject->DataArray1[1] = colour.Y;
		p_lightObject->DataArray1[2] = colour.Z;
	}
	
	
	Vec3 PointLight::GetColour()
	{
		return Vec3 (p_lightObject->DataArray1[0],
					 p_lightObject->DataArray1[1],
					 p_lightObject->DataArray1[2]);
	}
	
	
	void PointLight::SetIntensity(float intensity)
	{
		p_lightObject->DataArray1[3] = intensity;
	}
	
	
	float PointLight::GetIntensity()
	{
		return p_lightObject->DataArray1[3];
	}
	
	
	void PointLight::SetRange(float range)
	{
		p_lightObject->DataArray1[4] = range;
	}
	
	
	float PointLight::GetRange()
	{
		return p_lightObject->DataArray1[4];
	}
	
}