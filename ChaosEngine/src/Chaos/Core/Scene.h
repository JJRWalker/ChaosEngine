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
		virtual void FixedUpdate();
		virtual void EndScene();
		
		void AddEntity(Entity* entity) { m_entities.push_back(entity); }
		
		std::vector<Entity*>& GetEntities() { return m_entities; }
		protected:
		std::vector<Entity*> m_entities;
		std::vector<Entity*> m_collidableEntities;
	};
}