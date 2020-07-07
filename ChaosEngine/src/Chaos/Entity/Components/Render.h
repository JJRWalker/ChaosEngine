#pragma once
#include "chaospch.h"
#include "Chaos/Entity/Component.h"
#include "Chaos/Entity/Components/Transform.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Entity/Entity.h"

//Early implementation of a render component
//TODO: Add change colour functionality, Subtextures, tiling factor
namespace Chaos
{
	class Render : public IComponent
	{
	public:
		Render(Entity* owner) : m_entity(owner)
		{
		}
		Render() {};

		// Inherited via Component
		virtual void Start() override;
		virtual void Update() override;
		virtual void Destroy() override;
		virtual char* ToString() override { return "Render"; }

		void SetTexture(Ref<Texture> tex) { m_texture = tex; }
		Ref<Texture> GetTexture() { return m_texture; }
	private:
		Entity* m_entity = nullptr;
		Ref<Texture> m_texture = Texture::GetBlank();	//Setting default texture (loads blank)
	};
}