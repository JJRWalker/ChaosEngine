/* date = January 30th 2021 1:58 pm */

#ifndef _SPRITE_H
#define _SPRITE_H

#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Renderer/SubTexture.h"
#include "Chaos/Nodes/Node.h"

namespace Chaos
{
	class Sprite : public Node
	{
		public:
		Sprite();
		
		void Update(float delta) override;
		
		public:
		Ref<Texture> Texture = Texture::GetBlank();	//Setting default texture (loads blank)
		Vec4 Colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
	
	class SubSprite : public Node
	{
		public:
		SubSprite(){};
		virtual void Init() override;
		virtual void Update(float delta) override;
		virtual void Kill() override;
		
		void SetSubTexture(Ref<SubTexture> subTex) { m_subTex = subTex; }
		Ref<SubTexture>GetSubTexture() { return m_subTex; }
		
		Vec4& Colour() { return m_colour; }
		
		private:
		Ref<SubTexture> m_subTex = SubTexture::Create(Texture::GetBlank(), Vec2(0,0), Vec2(1,1));
		Vec4 m_colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
	
	class UISprite : public Node
	{
		public:
		UISprite(){};
		void Init() override;
		void Update(float delta) override;
		void Kill() override;
		
		void SetTexture(Ref<Texture> tex) { m_texture = tex; }
		Ref<Texture> GetTexture() { return m_texture; }
		
		Vec4& Colour() { return m_colour; }
		
		private:
		Ref<Texture> m_texture = Texture::GetBlank();	//init as a blank texture
		Vec4 m_colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}

#endif //_SPRITE_H
