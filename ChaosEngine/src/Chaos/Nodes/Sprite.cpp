#include "chaospch.h"

#include "Sprite.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Renderer/Material.h"

namespace Chaos
{
	// SPRITE
	Sprite::Sprite(bool child) : Node(child)
	{
		Name = "Sprite";
		Material = Material::Get("default");
		
		p_renderObject = Application::Get().GetRenderer().AddQuad(GetGlobalTransform(), Material);
	}
	
	
	Sprite::~Sprite()
	{
		Application::Get().GetRenderer().RemoveRenderable(p_renderObject);
	}
	
	
	void Sprite::OnUpdate(float delta)
	{
		// copy current global transform to the render object
		memcpy((void*)&p_renderObject->Transform[0], (void*)&GetGlobalTransform()[0], sizeof(float) * 16);
	}
	
	
	// SUB SPRITE
	SubSprite::SubSprite(bool child) : Node(child)
	{
		Name = "SubSprite";
		
		Material = Material::Get("default");
		
		p_renderObject = Application::Get().GetRenderer().AddQuad(GetGlobalTransform(), Material);
	}
	
	
	void SubSprite::OnStart()
	{
	}
	
	void SubSprite::OnUpdate(float delta)
	{
		memcpy((void*)&p_renderObject->Transform[0], (void*)&GetGlobalTransform()[0], sizeof(float) * 16);
	}
	
	void SubSprite::OnDestroy()
	{
		
	}
	
	// UI SPRITE
	UISprite::UISprite(bool child) : Node(child)
	{
		Name = "UISprite";
		Material = Material::Get("ui-default");
		
		p_renderObject = Application::Get().GetRenderer().AddQuad(GetGlobalTransform(), Material);
	}
	
	
	void UISprite::OnStart()
	{
		
	}
	
	void UISprite::OnUpdate(float delta)
	{
		memcpy((void*)&p_renderObject->Transform[0], (void*)&GetGlobalTransform()[0], sizeof(float) * 16);
	}
	
	void UISprite::OnDestroy()
	{
		
	}
}