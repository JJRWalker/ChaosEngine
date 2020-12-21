#include "chaospch.h"
#include "Scene.h"
#include "Chaos/Entity/Components/Collider.h"

namespace Chaos
{
	void Scene::StartScene()
	{
		for (auto* entity : m_entities)
		{
			entity->Start();
		}
	}
	void Scene::Update()
	{
		PROFILED_FUNC();
		
		for (auto* entity : m_entities)
		{
			entity->Update();
			
			if (entity->HasComponent<Collider>())
			{
				m_collidableEntities.push_back(entity);
			}
		}
		
		//update colisions
		for (int i = 0; i < m_collidableEntities.size(); ++i)
		{
			//only need to update collisions with the items that come after in the list as we don't need to check the same two entities more than once
			for (int j = i + 1; j < m_collidableEntities.size(); ++j)
			{
				for (auto& collider : m_collidableEntities[i]->GetAllComponentsByType<Collider>())
				{
					collider->CollideWith(*m_collidableEntities[j]->GetComponent<Collider>());
				}
			}
		}
		//Clearing collidables to be readded next update
		//not really efficent, but if we add colliders during runtime they won't be added to the list yet...
		//TODO: improve efficentcy by adding to this vector only when a new collider component is added
		m_collidableEntities.clear();
	}
	void Scene::FixedUpdate()
	{
		for (auto* entity : m_entities)
		{
			entity->FixedUpdate();
		}
	}
	void Scene::EndScene()
	{
		for (auto* entity : m_entities)
		{
			entity->Destroy();
		}
		m_entities.clear();
	}
}