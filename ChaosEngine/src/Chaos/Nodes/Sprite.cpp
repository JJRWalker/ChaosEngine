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
		
		p_renderObject = Application::Get().GetRenderer().AddQuad(GetWorldTransform(), Material::Get("textured-default"));
		memset(p_renderObject->ShaderData.DataArray1, 0, sizeof(float) * 16);
		memset(p_renderObject->ShaderData.CustomDataArray1, 0, sizeof(float) * 16);
		SetColour(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	
	
	Sprite::Sprite(Vec2 position, bool child) : Node(child)
	{
		Name = "Sprite";
		
		SetPosition(position);
		
		p_renderObject = Application::Get().GetRenderer().AddQuad(GetWorldTransform(), Material::Get("textured-default"));
		memset(p_renderObject->ShaderData.DataArray1, 0, sizeof(float) * 16);
		memset(p_renderObject->ShaderData.CustomDataArray1, 0, sizeof(float) * 16);
		SetColour(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	
	
	Sprite::Sprite(Vec3 position, bool child) : Node(child)
	{
		Name = "Sprite";
		
		SetPosition(position);
		
		p_renderObject = Application::Get().GetRenderer().AddQuad(GetWorldTransform(), Material::Get("textured-default"));
		memset(p_renderObject->ShaderData.DataArray1, 0, sizeof(float) * 16);
		memset(p_renderObject->ShaderData.CustomDataArray1, 0, sizeof(float) * 16);
		SetColour(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	
	
	Sprite::~Sprite()
	{
		Application::Get().GetRenderer().RemoveRenderable(p_renderObject);
	}
	
	
	void Sprite::OnUpdate(float delta)
	{
		if (UpdateType == SpriteUpdateType::DYNAMIC)
			// copy current global transform to the render object
			memcpy((void*)&p_renderObject->ShaderData.Transform[0], (void*)&GetWorldTransform()[0], sizeof(float) * 16);
	}
	
	
	void Sprite::SetEnabled(bool state)
	{
		if (Enabled == state)
			return;
		Enabled = state;
		if (!state)
			Application::Get().GetRenderer().RemoveRenderable(p_renderObject);
		else
			Application::Get().GetRenderer().AddRenderable(p_renderObject);
	}
	
	
	void Sprite::SetMaterial(Material* mat) 
	{
		p_renderObject->pMaterial = mat; 
	}
	
	
	Material* Sprite::GetMaterial()
	{
		return p_renderObject->pMaterial;
	}
	
	
	void Sprite::SetTexture(Texture* tex)
	{
		p_renderObject->pTexture = tex;
	}
	
	
	Texture* Sprite::GetTexture()
	{
		return p_renderObject->pTexture;
	}
	
	
	void Sprite::SetColour(Vec4 colour)
	{
		memcpy((void*)&p_renderObject->ShaderData.DataArray1, (void*)&colour, sizeof(float) * 4);
		
	}
	
	
	Vec4 Sprite::GetColour()
	{
		return Vec4(p_renderObject->ShaderData.DataArray1[0],
					p_renderObject->ShaderData.DataArray1[1], p_renderObject->ShaderData.DataArray1[2], p_renderObject->ShaderData.DataArray1[3]);
	}
	
	
	void Sprite::SetShaderFloatData(size_t index, float value)
	{
		p_renderObject->ShaderData.DataArray1[index] = value;
	}
	
	
	// SUB SPRITE
	SubSprite::SubSprite(bool child) : Sprite(child)
	{
		Name = "SubSprite";
		
		SetMaterial(Material::Get("subsprite-default"));
		SetTilingFactor(Vec2(1.0f, 1.0f));
	}
	
	
	SubSprite::SubSprite(Vec2 position, bool child) : Sprite(position, child)
	{
		Name = "SubSprite";
		
		SetMaterial(Material::Get("subsprite-default"));
		SetTilingFactor(Vec2(1.0f, 1.0f));
	}
	
	
	SubSprite::SubSprite(Vec3 position, bool child) : Sprite(position, child)
	{
		Name = "SubSprite";
		
		SetMaterial(Material::Get("subsprite-default"));
		SetTilingFactor(Vec2(1.0f, 1.0f));
	}
	
	
	// not keen on this approach. Kinda removes visibility from what's actually going on.
	// not explicit that shader data array index 0 and 1 are used by the subsprite material
	// could be reused by the user and cause issues
	void SubSprite::SetCoords(Vec2 coords)
	{
		SetShaderFloatData(4, coords.X);
		SetShaderFloatData(5, coords.Y);
	}
	
	
	void SubSprite::SetTotalCells(Vec2 dimensions)
	{
		SetShaderFloatData(6, dimensions.X);
		SetShaderFloatData(7, dimensions.Y);
	}
	
	
	void SubSprite::SetTilingFactor(Vec2 tilingFactor)
	{
		SetShaderFloatData(8, tilingFactor.X);
		SetShaderFloatData(9, tilingFactor.Y);
	}
	
	
	Vec2 SubSprite::GetCoords()
	{
		return Vec2(p_renderObject->ShaderData.DataArray1[4], p_renderObject->ShaderData.DataArray1[5]);
	}
	
	
	Vec2 SubSprite::GetTotalCells()
	{
		return Vec2(p_renderObject->ShaderData.DataArray1[6], p_renderObject->ShaderData.DataArray1[7]);
	}
	
	
	Vec2 SubSprite::GetTilingFactor()
	{
		return Vec2(p_renderObject->ShaderData.DataArray1[8], p_renderObject->ShaderData.DataArray1[9]);
	}
	
	
	// UI SPRITE
	UISprite::UISprite(bool child) : Sprite(child)
	{
		Name = "UISprite";
		SetMaterial(Material::Get("ui-default"));
	}
}
