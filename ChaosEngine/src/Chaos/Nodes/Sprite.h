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
		Sprite(bool child = false);
		
		void OnUpdate(float delta) override;
		
		public:
		Ref<Texture> Texture = Texture::Create();	//Setting default texture (loads blank)
		Vec4 Colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
	
	class SubSprite : public Node
	{
		public:
		SubSprite(bool child = false) : Node(child) {Name = "SubSprite";}
		virtual void OnStart() override;
		virtual void OnUpdate(float delta) override;
		virtual void OnDestroy() override;
		
		void SetSubTexture(Ref<SubTexture> subTex) { m_subTex = subTex; }
		Ref<SubTexture>GetSubTexture() { return m_subTex; }
		
		Vec4& Colour() { return m_colour; }
		
		private:
		Ref<SubTexture> m_subTex = SubTexture::Create(Texture::Create(), Vec2(0,0), Vec2(1,1));
		Vec4 m_colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
	
	class UISprite : public Node
	{
		public:
		UISprite(bool child = false) : Node(child) { Name = "UISprite"; }
		void OnStart() override;
		void OnUpdate(float delta) override;
		void OnDestroy() override;
		
		void SetTexture(Ref<Texture> tex) { m_texture = tex; }
		Ref<Texture> GetTexture() { return m_texture; }
		
		Vec4& Colour() { return m_colour; }
		
		private:
		Ref<Texture> m_texture = Texture::Create();	//init as a blank texture
		Vec4 m_colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}

#endif //_SPRITE_H
