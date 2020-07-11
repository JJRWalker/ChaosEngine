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
		Application::Get().GetRenderer().DrawQuad(GetEntity()->GetTransform()->Position(), GetEntity()->GetTransform()->Scale(), GetEntity()->GetTransform()->Rotation(), m_texture);
	}

	void Render::Destroy() 
	{
		 
	}
}