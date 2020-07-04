#include "chaospch.h"
#include "Render.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

namespace Chaos
{
	void Render::Start() const
	{
	}

	void Render::Update() const
	{
		Application::Get().GetRenderer().DrawQuad(m_entity->GetTransform()->Position(), m_entity->GetTransform()->Scale(), m_texture);
	}

	void Render::Destroy() const
	{
		 
	}
}