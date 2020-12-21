#include "chaospch.h"
#include "Chaos/Entity/ECSManager.h"
#include "Component.h"
#include "Entity.h"
#include "Chaos/Core/SceneManager.h"

namespace Chaos
{
	uint32_t ECSManager::s_entityID = 0;
	uint32_t ECSManager::s_componentID = 0;
	std::vector<uint32_t> ECSManager::s_empty;
	std::unordered_map<uint32_t, std::vector<uint32_t>> ECSManager::s_entityComponentMap;	//stores an entity instance ID and the Component ID 
	std::unordered_map<uint32_t, uint32_t> ECSManager::s_componentEntityMap;			//stores the same data but the component ID is the key 
	std::unordered_map<uint32_t, Entity*> ECSManager::s_idEntityMap;
	std::unordered_map<uint32_t, Component*> ECSManager::s_idComponentMap;
	
	void ECSManager::AddComponent(uint32_t entityID, Component* component)
	{
		component->m_componentID = s_componentID;
		++s_componentID;
		std::unordered_map<uint32_t, std::vector<uint32_t>>::iterator it = s_entityComponentMap.find(entityID);
		
		//if the entity already exists in this map
		if (it != s_entityComponentMap.end())
		{
			//get the components on entity and push id to the back, then add that to the id-component map
			GetComponentsOnEntity(entityID).push_back(component->m_componentID);
			s_idComponentMap.insert({ component->m_componentID, component });
			s_componentEntityMap.insert({ component->m_componentID, entityID });
		}
		else
		{
			//if not present then something has gone seriously wrong, entities are added on construction. Therefore no entity should exist and not be in this system
			LOGCORE_ERROR("ECS: entityID not found in system, something has gone seriously wrong, entities are added to the system on construction!");
		}
	}
	void ECSManager::AddEntity(Entity* entity)
	{
		entity->m_entityID = s_entityID;
		++s_entityID;
		s_idEntityMap.insert({ entity->m_entityID, entity });
		s_entityComponentMap.insert({ entity->m_entityID, std::vector<uint32_t>() });
		SceneManager::GetScene()->AddEntity(entity);	//automatically add the entity being created to the current scene
	}
	
	std::vector<uint32_t>& ECSManager::GetComponentsOnEntity(uint32_t entityID)
	{
		std::unordered_map<uint32_t, std::vector<uint32_t>>::iterator it = s_entityComponentMap.find(entityID);
		if (it != s_entityComponentMap.end())
		{
			return it->second;
		}
		LOGCORE_WARN("ECS: entity has no components associated with it, returning empty container");
		return s_empty;
	}
	
	
	std::vector<Component*> ECSManager::GetComponentsFromIDs(std::vector<uint32_t> componentIDs)
	{
		std::vector<Component*> toReturn;
		std::unordered_map<uint32_t, Component*>::iterator it;
		for (size_t i = 0; i < componentIDs.size(); ++i)
		{
			it = s_idComponentMap.find(componentIDs[i]);
			if (it != s_idComponentMap.end())
			{
				toReturn.push_back(it->second);
			}
		}
		return toReturn;
	}
	
	Entity* ECSManager::GetEntityFromComponent(uint32_t componentID)
	{
		std::unordered_map<uint32_t, uint32_t>::iterator it = s_componentEntityMap.find(componentID);
		
		if (it != s_componentEntityMap.end())
		{
			//if the component has an entity attached (it should always have one)
			std::unordered_map<uint32_t, Entity*>::iterator entityIt = s_idEntityMap.find(it->second);
			if (entityIt != s_idEntityMap.end())
			{
				return entityIt->second;
			}
		}
		LOGCORE_ERROR("ECS: no entity associated with component ID, this should never happen. Returned nullptr");
		return nullptr;
	}
}