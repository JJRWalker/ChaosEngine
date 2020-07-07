#pragma once
#include "chaospch.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Entity/Entity.h"

namespace Chaos
{
	class Scene
	{
	public:
		virtual void StartScene();
		virtual void Update();
		virtual void EndScene();

		std::vector<Entity*>& GetEntities() { return m_entities; }
	protected:
		std::vector<Entity*> m_entities;
	};
}