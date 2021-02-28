#include "chaospch.h"

#include "Sprite.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

namespace Chaos
{
	// SPRITE
	Sprite::Sprite()
	{
	}
	
	
	void Sprite::Update(float delta)
	{
		Application::Get().GetRenderer().DrawQuad(GetPosition3D(), GetScale(), Vec2(GetRotation(), GetRotation()), Colour, Texture);
	}
	
	// SUB SPRITE
	void SubSprite::Init()
	{
	}
	
	void SubSprite::Update(float delta)
	{
		Application::Get().GetRenderer().DrawQuad(GetPosition3D(), GetScale(), Vec2(GetRotation(), GetRotation()),
												  m_colour, m_subTex);
	}
	
	void SubSprite::Kill()
	{
		
	}
	
	// UI SPRITE
	void UISprite::Init()
	{
		
	}
	
	void UISprite::Update(float delta)
	{
		Application::Get().GetRenderer().DrawScreenSpaceQuad(GetPosition3D(), GetScale(), 
															 Vec2(GetRotation(), GetRotation()), m_colour, m_texture, 1);
	}
	
	void UISprite::Kill()
	{
		
	}
}