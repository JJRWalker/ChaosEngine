#include "chaospch.h"
#include "Render.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

namespace Chaos
{
	void Render::Start() 
	{
	}

	void Render::Update() 
	{
		Application::Get().GetRenderer().DrawQuad(m_entity->GetTransform()->Position(),  m_entity->GetTransform()->Scale(), m_entity->GetTransform()->Rotation(), m_texture);
	}

	void Render::Destroy() 
	{
		 
	}
}