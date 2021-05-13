/* date = January 30th 2021 1:58 pm */

#ifndef _SPRITE_H
#define _SPRITE_H

#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Renderer/SubTexture.h"
#include "Chaos/Nodes/Node.h"

namespace Chaos
{
	struct RenderObject;
	class Material;
	class Sprite : public Node
	{
		public:
		Sprite(bool child = false);
		~Sprite();
		
		void OnUpdate(float delta) override;
		
		void SetMaterial(Material* mat);
		Material* GetMaterial();
		
		void SetShaderFloatData(size_t index, float value);
		
		public:
		Texture* Texture = Texture::GetBlank();	//Setting default texture (loads blank)
		Vec4 Colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		protected:
		Material* p_material = nullptr;
		RenderObject* p_renderObject = nullptr;
	};
	
	class SubSprite : public Sprite
	{
		public:
		SubSprite(bool child = false);
		
		void SetCoords(Vec2 coords);
		void SetTotalCells(Vec2 dimensions); // total number of cells in x and y. NOT pixel size of grid
		Vec2 GetTotalCells();
		Vec2 GetCoords();
	};
	
	class UISprite : public Sprite
	{
		public:
		UISprite(bool child = false);
	};
}

#endif //_SPRITE_H
