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
	class Render : public Component
	{
	public:
		Render(Entity* owner) : mEntity(owner)
		{
		}
		Render() {};

		// Inherited via Component
		virtual void Start() const override;
		virtual void Update() const override;
		virtual void Destroy() const override;

		void SetTexture(Ref<Texture> tex) { mTexture = tex; }
	private:
		Entity* mEntity = nullptr;
		Ref<Texture> mTexture = Texture::Create("");	//Setting default texture (loads blank)
	};
}