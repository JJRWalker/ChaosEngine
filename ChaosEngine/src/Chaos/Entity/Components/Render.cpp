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
		Application::Get().GetRenderer().DrawQuad(mEntity->GetTransform()->Position(), mEntity->GetTransform()->Scale(), mTexture);
	}

	void Render::Destroy() const
	{
		delete mEntity;
	}
}