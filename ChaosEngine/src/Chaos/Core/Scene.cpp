#include "chaospch.h"
#include "Scene.h"
#include "Chaos/Entity/Components/Collider.h"

#include <algorithm>

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
		
		// update colisions
		for (int i = 0; i < m_entities.size(); ++i)
		{
			m_entities[i]->Update();
			
			// only need to update collisions with the items that come after in the list as we don't need to check the same two entities more than once
			for (int j = i + 1; j < m_entities.size(); ++j)
			{
				for (auto& collider : m_entities[i]->GetAllComponentsByType<Collider>())
				{
					std::vector<Collider*> uncollided;
					for (auto& other : m_entities[j]->GetAllComponentsByType<Collider>())
					{
						if(collider->CollideWith(*other))
						{
							std::vector<Collider*>& colliderOverlaps = collider->GetOverlaps();
							std::vector<Collider*>& otherOverlaps = other->GetOverlaps();
							
							if (std::find(colliderOverlaps.begin(), colliderOverlaps.end(), other) == colliderOverlaps.end())
							{
								if (collider->IsTrigger())
								{
									m_entities[i]->TriggerEnter(collider, other);
									m_entities[j]->TriggerEnter(other, collider);
								}
								else
								{
									m_entities[i]->ColliderEnter(collider, other);
									m_entities[j]->ColliderEnter(other, collider);
								}
								
								colliderOverlaps.push_back(other);
								otherOverlaps.push_back(collider);
							}
						}
						else
						{
							std::vector<Collider*>::iterator found = std::find(collider->GetOverlaps().begin(), collider->GetOverlaps().end(), other);
							if (found != collider->GetOverlaps().end())
							{
								if (collider->IsTrigger())
								{
									m_entities[i]->TriggerExit(collider, other);
									other->GetEntity()->TriggerExit(other, collider);
								}
								else
								{
									m_entities[i]->ColliderExit(collider, other);
									other->GetEntity()->ColliderExit(other, collider);
								}
								std::vector<Collider*>& foundOverlaps = other->GetOverlaps();
								std::vector<Collider*>::iterator selfIt = std::find(foundOverlaps.begin(), foundOverlaps.end(), collider);
								foundOverlaps.erase(selfIt);
								collider->GetOverlaps().erase(found);
							}
						}
					}
				}
			}
		}
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