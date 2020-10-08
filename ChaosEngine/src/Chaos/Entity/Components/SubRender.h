/* date = October 7th 2020 5:34 pm */

#ifndef _SUB_RENDER_H
#define _SUB_RENDER_H

#include <Chaos/Entity/Component.h>
#include <Chaos/Renderer/SubTexture.h>
#include <Chaos/DataTypes/Vec4.h>

namespace Chaos
{
	class SubRender : public Component
	{
		public:
		SubRender(){};
		virtual void Start() override;
		virtual void Update() override;
		virtual void Destroy() override;
		
		void SetSubTexture(Ref<SubTexture> subTex) { m_subTex = subTex; }
		Ref<SubTexture>GetSubTexture() { return m_subTex; }
		
		Vec4& Colour() { return m_colour; }
		
		private:
		Ref<SubTexture> m_subTex = SubTexture::Create(Texture::GetBlank(), Vec2(0,0), Vec2(1,1));
		Vec4 m_colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}

#endif //_SUB_RENDER_H
