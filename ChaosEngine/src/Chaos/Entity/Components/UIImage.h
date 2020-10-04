#pragma once
#include "Chaos/Entity/Component.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/DataTypes/Vec4.h"

namespace Chaos
{
	class UIImage : public Component
	{
	public:
		void Start() override;
		void Update() override;
		void Destroy() override;

		void SetTexture(Ref<Texture> tex) { m_texture = tex; }
		Ref<Texture> GetTexture() { return m_texture; }

		Vec4& Colour() { return m_colour; }

	private:
		Ref<Texture> m_texture = Texture::GetBlank();	//init as a blank texture
		Vec4 m_colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}