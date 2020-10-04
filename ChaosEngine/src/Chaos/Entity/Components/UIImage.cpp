#include "chaospch.h"
#include "UIImage.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Entity/Entity.h"

namespace Chaos
{
	void UIImage::Start()
	{

	}

	void UIImage::Update()
	{
		Application::Get().GetRenderer().DrawScreenSpaceQuad(GetEntity()->GetTransform()->Position(), GetEntity()->GetTransform()->Scale(), 
			GetEntity()->GetTransform()->Rotation(), m_colour, m_texture, 1);
	}

	void UIImage::Destroy()
	{

	}
}