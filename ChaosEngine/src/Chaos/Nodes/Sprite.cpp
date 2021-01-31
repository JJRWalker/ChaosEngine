#include "chaospch.h"

#include "Sprite.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

namespace Chaos
{
	Sprite::Sprite()
	{
	}
	
	
	void Sprite::Update(float delta)
	{
		Application::Get().GetRenderer().DrawQuad(GetPosition3D(), GetScale(), Vec2(GetRotation(), GetRotation()), Colour, Texture);
	}
}