/* date = January 30th 2021 1:58 pm */
/* Main sprite class is essentially a wrapper around a render object that can be stored as a node or child node in the level
 * this also provides functions that are more intuitive for a user hopefully making things clearer.
*/

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
		void SetEnabled(bool state) override;
		
		void SetMaterial(Material* mat);
		Material* GetMaterial();
		
		void SetTexture(Texture* tex);
		Texture* GetTexture();
		
		void SetShaderFloatData(size_t index, float value);
		
		public:
		Vec4 Colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		protected:
		RenderObject* p_renderObject = nullptr;
	};
	
	class SubSprite : public Sprite
	{
		public:
		SubSprite(bool child = false);
		
		void SetCoords(Vec2 coords);
		void SetTotalCells(Vec2 dimensions); // total number of cells in x and y. NOT pixel size of grid
		void SetTilingFactor(Vec2 tilingFactor);
		
		Vec2 GetTotalCells();
		Vec2 GetCoords();
		Vec2 GetTilingFactor();
	};
	
	class UISprite : public Sprite
	{
		public:
		UISprite(bool child = false);
	};
}

#endif //_SPRITE_H
