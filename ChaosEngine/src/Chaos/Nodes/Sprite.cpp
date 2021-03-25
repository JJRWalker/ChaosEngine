#include "chaospch.h"

#include "Sprite.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

namespace Chaos
{
	// SPRITE
	Sprite::Sprite(bool child) : Node(child)
	{
		Name = "Sprite";
	}
	
	
	void Sprite::OnUpdate(float delta)
	{
		Application::Get().GetRenderer().DrawQuad(GetPosition3D(), GetScale(), Vec2(GetRotation(), GetRotation()), Colour, Texture);
	}
	
	// SUB SPRITE
	void SubSprite::OnStart()
	{
	}
	
	void SubSprite::OnUpdate(float delta)
	{
		Application::Get().GetRenderer().DrawQuad(GetPosition3D(), GetScale(), Vec2(GetRotation(), GetRotation()),
												  m_colour, m_subTex);
	}
	
	void SubSprite::OnDestroy()
	{
		
	}
	
	// UI SPRITE
	void UISprite::OnStart()
	{
		
	}
	
	void UISprite::OnUpdate(float delta)
	{
		Application::Get().GetRenderer().DrawScreenSpaceQuad(GetPosition3D(), GetScale(), 
															 Vec2(GetRotation(), GetRotation()), m_colour, m_texture, 1);
	}
	
	void UISprite::OnDestroy()
	{
		
	}
}