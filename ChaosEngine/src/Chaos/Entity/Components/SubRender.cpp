#include <chaospch.h>
#include <Chaos/Entity/Components/SubRender.h>
#include <Chaos/Core/Application.h>
#include <Chaos/Renderer/Renderer.h>
#include <Chaos/Entity/Entity.h>

namespace Chaos
{
	void SubRender::Start()
	{
	}
	
	void SubRender::Update()
	{
		Application::Get().GetRenderer().DrawQuad(GetEntity()->GetTransform()->Position(), GetEntity()->GetTransform()->Scale(), GetEntity()->GetTransform()->Rotation(),
												  m_colour, m_subTex);
	}
	
	void SubRender::Destroy()
	{
		
	}
	
}