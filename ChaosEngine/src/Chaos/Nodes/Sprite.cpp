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
		
		
		if (!p_material)
			p_material = Material::Get("default");
		
		p_renderObject = Application::Get().GetRenderer().AddQuad(GetGlobalTransform(), p_material);
		
		p_renderObject->pTexture = p_texture;
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
	
	
	void Sprite::SetMaterial(Material* mat) 
	{
		p_renderObject->pMaterial = mat; 
		p_material = mat;
	}
	
	
	Material* Sprite::GetMaterial()
	{
		return p_material;
	}
	
	
	void Sprite::SetTexture(Texture* tex)
	{
		p_texture = tex;
		p_renderObject->pTexture = tex;
	}
	
	
	Texture* Sprite::GetTexture()
	{
		return p_texture;
	}
	
	
	void Sprite::SetShaderFloatData(size_t index, float value)
	{
		p_renderObject->ShaderDataArray1[index] = value;
	}
	
	
	// SUB SPRITE
	SubSprite::SubSprite(bool child) : Sprite(child)
	{
		Name = "SubSprite";
		
		SetMaterial(Material::Get("subsprite-default"));
	}
	
	
	// not keen on this approach. Kinda removes visibility from what's actually going on.
	// not explicit that shader data array index 0 and 1 are used by the subsprite material
	// could be reused by the user and cause issues
	void SubSprite::SetCoords(Vec2 coords)
	{
		SetShaderFloatData(0, coords.X);
		SetShaderFloatData(1, coords.Y);
	}
	
	
	void SubSprite::SetTotalCells(Vec2 dimensions)
	{
		SetShaderFloatData(2, dimensions.X);
		SetShaderFloatData(3, dimensions.Y);
	}
	
	
	Vec2 SubSprite::GetCoords()
	{
		return Vec2(p_renderObject->ShaderDataArray1[0], p_renderObject->ShaderDataArray1[1]);
	}
	
	
	Vec2 SubSprite::GetTotalCells()
	{
		return Vec2(p_renderObject->ShaderDataArray1[2], p_renderObject->ShaderDataArray1[3]);
	}
	
	
	// UI SPRITE
	UISprite::UISprite(bool child) : Sprite(child)
	{
		Name = "UISprite";
		SetMaterial(Material::Get("ui-default"));
	}
}
