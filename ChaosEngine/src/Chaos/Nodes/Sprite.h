/* date = January 30th 2021 1:58 pm */
/* Main sprite class is essentially a wrapper around a render object that can be stored as a node or child node in the level
 * this also provides functions that are more intuitive for a user hopefully making things clearer.
*/

#ifndef _SPRITE_H
#define _SPRITE_H

#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Nodes/Node.h"

namespace Chaos
{
	struct RenderObject;
	class Material;
	
	enum SpriteUpdateType
	{
		DYNAMIC      = 0,
		STATIC       = 1
	};
	
	class Sprite : public Node
	{
		public:
		Sprite();
		Sprite(Vec2 position);
		Sprite(Vec3 position);
		~Sprite();
		
		// Node overrides
		void OnUpdate(float delta) override;
		void SetEnabled(bool state) override;
		
		virtual Binary SaveToBinary() override;
		virtual size_t LoadFromBinary(char* data);
		
		virtual void OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle);
		
		// Actual sprite stuff
		void SetMaterial(Material* mat);
		Material* GetMaterial();
		
		void SetTexture(Texture* tex);
		Texture* GetTexture();
		
		void SetColour(Vec4 colour);
		Vec4 GetColour();
		
		void SetShaderFloatData(size_t index, float value);
		
		
		public:
		SpriteUpdateType UpdateType = SpriteUpdateType::DYNAMIC;
		
		protected:
		RenderObject* p_renderObject = nullptr;
	};
	
	class SubSprite : public Sprite
	{
		public:
		SubSprite();
		SubSprite(Vec2 position);
		SubSprite(Vec3 position);
		
		Binary SaveToBinary() override;
		size_t LoadFromBinary(char* data);
		virtual void OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle);
		
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
		UISprite();
	};
}

#endif //_SPRITE_H
