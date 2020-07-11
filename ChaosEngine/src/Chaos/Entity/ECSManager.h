#pragma once
#include <unordered_map>
#include "Chaos/Core/Log.h"

namespace Chaos
{
	class Entity;
	class Component;
	class ECSManager
	{
	public:

		static void Load();
		static void AddComponent(uint32_t entityID, Component* component);
		static void AddEntity(Entity* entity);

		//finds entity ID in entity component map and returns the list of component IDs asssociated with it, if none returns empty container
		static std::vector<uint32_t>& GetComponentsOnEntity(uint32_t entityID);
		//goes through each ID, tries to find the component in the map, if it finds it it adds it to a vector to return, will return empty container if none are present
		static std::vector<Component*> GetComponentsFromIDs(std::vector<uint32_t> componentIDs);
		//searches the component entity map for the entity ID associated with that component ID, then gets the entity object from the ID entity map
		static Entity* GetEntityFromComponent(uint32_t componentID);
	private:
		static std::unordered_map<uint32_t, std::vector<uint32_t>> s_entityComponentMap;	//stores an entity instance ID and the Component ID 
		static std::unordered_map<uint32_t, uint32_t> s_componentEntityMap;			//stores the same data but the component ID is the key 
		static std::unordered_map<uint32_t, Entity*> s_idEntityMap;
		static std::unordered_map<uint32_t, Component*> s_idComponentMap;

		static uint32_t s_componentID;
		static uint32_t s_entityID;
	};
}