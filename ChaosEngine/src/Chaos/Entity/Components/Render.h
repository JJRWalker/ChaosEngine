#pragma once
#include "chaospch.h"
#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/Entity/Component.h"
#include "Chaos/Entity/Components/Transform.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Entity/Entity.h"

//Early implementation of a render component
//TODO: Add change colour functionality, Subtextures, tiling factor
namespace Chaos
{
	class Render : public Component
	{
		public:
		Render() {};
		// Inherited via Component
		virtual void Start() override;
		virtual void Update() override;
		virtual void Destroy() override;
		
		void SetTexture(Ref<Texture> tex) { m_texture = tex; }
		Ref<Texture> GetTexture() { return m_texture; }
		
		Vec4& Colour() { return m_colour; }	//simple getter / setter for colour. can be modified using this reference from outside the class
		
		private:
		Ref<Texture> m_texture = Texture::GetBlank();	//Setting default texture (loads blank)
		Vec4 m_colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}
