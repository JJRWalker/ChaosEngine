#include "chaospch.h"

#include "Lights.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

#include "Chaos/Debug/ImGuiEditor.h"

namespace Chaos
{
	PointLight2D::PointLight2D(bool child) : Node(child)
	{
		Name = "Point-light-2D";
		
		p_lightObject = Application::Get().GetRenderer().AddLight(GetWorldTransform());
		
		memcpy((void*)&p_lightObject->DataArray1[0], (void*)&DEFAULT_SHADER_VALUES_1[0], sizeof(float) * 16);
	}
	
	
	PointLight2D::~PointLight2D()
	{
		Application::Get().GetRenderer().RemoveLight(p_lightObject);
		delete p_lightObject;
	}
	
	
	void PointLight2D::OnUpdate(float delta)
	{
		memcpy((void*)&p_lightObject->Transform[0], (void*)&GetWorldTransform()[0], sizeof(float) * 16);
	}
	
	
	void PointLight2D::SetEnabled(bool state)
	{
		if (Enabled == state)
			return;
		
		Enabled = state;
		if (!state)
			p_lightObject->DataArray1[15] = 0.0f;
		if (state)
			p_lightObject->DataArray1[15] = 1.0f;
	}
	
	
	void PointLight2D::OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle)
	{
		float colour[3] = { GetColour().X, GetColour().Y, GetColour().Z };
		float range = GetRange();
		float intensity = GetIntensity();
		
		if (ImGui::ColorEdit3("Colour", colour, 0.01f))
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
	
	
	void PointLight2D::SetColour(Vec3 colour)
	{
		p_lightObject->DataArray1[0] = colour.X;
		p_lightObject->DataArray1[1] = colour.Y;
		p_lightObject->DataArray1[2] = colour.Z;
	}
	
	
	Vec3 PointLight2D::GetColour()
	{
		return Vec3 (p_lightObject->DataArray1[0],
					 p_lightObject->DataArray1[1],
					 p_lightObject->DataArray1[2]);
	}
	
	
	void PointLight2D::SetIntensity(float intensity)
	{
		p_lightObject->DataArray1[3] = intensity;
	}
	
	
	float PointLight2D::GetIntensity()
	{
		return p_lightObject->DataArray1[3];
	}
	
	
	void PointLight2D::SetRange(float range)
	{
		p_lightObject->DataArray1[4] = range;
	}
	
	
	float PointLight2D::GetRange()
	{
		return p_lightObject->DataArray1[4];
	}
	
}