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
		
		public:
		Texture* Texture = Texture::GetBlank();	//Setting default texture (loads blank)
		Vec4 Colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		private:
		Material* p_material = nullptr;
		RenderObject* p_renderObject = nullptr;
	};
	
	class SubSprite : public Node
	{
		public:
		SubSprite(bool child = false);
		virtual void OnStart() override;
		virtual void OnUpdate(float delta) override;
		virtual void OnDestroy() override;
		
		void SetMaterial(Material* mat);
		Material* GetMaterial();
		
		void SetSubTexture(Ref<SubTexture> subTex) { m_subTex = subTex; }
		Ref<SubTexture>GetSubTexture() { return m_subTex; }
		
		Vec4& Colour() { return m_colour; }
		
		private:
		Ref<SubTexture> m_subTex = SubTexture::Create(Texture::GetBlank(), Vec2(0,0), Vec2(1,1));
		Vec4 m_colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		RenderObject* p_renderObject = nullptr;
		Material* p_material = nullptr;
	};
	
	class UISprite : public Node
	{
		public:
		UISprite(bool child = false);
		void OnStart() override;
		void OnUpdate(float delta) override;
		void OnDestroy() override;
		
		void SetMaterial(Material* mat);
		Material* GetMaterial();
		
		void SetTexture(Texture* tex) { m_texture = tex; }
		Texture* GetTexture() { return m_texture; }
		
		Vec4& Colour() { return m_colour; }
		
		private:
		Texture* m_texture = Texture::GetBlank();	//init as a blank texture
		Vec4 m_colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		RenderObject* p_renderObject = nullptr;
		Material* p_material = nullptr;
	};
}

#endif //_SPRITE_H
