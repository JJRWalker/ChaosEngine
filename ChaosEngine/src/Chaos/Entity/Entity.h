#pragma once
#include "chaospch.h"
#include "Components/Transform.h"	//Only components included are those that do not have a reference to their owner
#include "ECSManager.h"

namespace Chaos
{
	//static count of all entities, incremented each time an entity is created. Can be used to limit how many entities can be created in the future
	static size_t sEntityCount = 0;
	static const size_t MAX_ENTITY_NAME_LENGTH = 128;

	class Entity
	{
		friend class ECSManager;

	public:
		Entity() : m_name("entity") { ECSManager::AddEntity(this); ECSManager::AddComponent(m_entityID, m_transform); }
		Entity(char* name) { strcpy_s(m_name, name); ECSManager::AddEntity(this); ECSManager::AddComponent(m_entityID, m_transform); }
		~Entity()
		{
			Destroy();
		}

		void Start() 
		{
			for (auto* c : GetAllComponents())
			{
				c->Start();
			}
		}
		void Update() 
		{
			for (auto* c : GetAllComponents())
			{
				c->Update();
			}
		}
		void Destroy()
		{
			for (auto* c : GetAllComponents())
			{
				c->Destroy();
			}
		}

		//Checks if the given class is derived from component, if so then it initalises a new class of that type and adds it to the vector, else print a warning to console
		template<typename T>
		void AddComponent()
		{
			if (std::is_base_of<Component, T>::value)
			{
				T* component = new T();
				ECSManager::AddComponent(m_entityID, component);
			}
			else
			{
				LOGCORE_WARN("Tried to add a component that was not derived from Component");
			}
		}

		template<typename T>
		T* GetComponent()
		{
			std::vector<Component*> components = ECSManager::GetComponentsFromIDs(ECSManager::GetComponentsOnEntity(m_entityID));
			for (Component* c : components)
			{
				T* component = dynamic_cast<T*>(c); //this will return a nullptr if the cast was not possible
				if (component != nullptr)
				{					 
					return component;
				}
			}
			LOGCORE_ERROR("Failed to get component on entity {0} ID: {1} returning nullptr!", m_name, m_entityID);
			return nullptr;
		}

		///itterates through all components on the entity and trys to cast them to the component type provided as <T> returns true and modifies outComponent input if found, else returns false
		template<typename T>
		bool TryGetComponent(T* outComponent)
		{
			if (std::is_base_of<Component, T>::value)
			{
				std::vector<Component*> components = ECSManager::GetComponentsFromIDs(ECSManager::GetComponentsOnEntity(m_entityID));
				for (Component* c : components)
				{
					T* component = dynamic_cast<T*>(c); //this will return a nullptr if the cast was not possible
					if (component != nullptr)
					{
						outComponent = component;
						return true;
					}
				}
				LOGCORE_WARN("Component could not be found on Entity: {1} ID: {2}", m_name, m_entityID);
				return false;
			}
			LOGCORE_WARN("Component given to template TryGetComponent() is not derived from type component");
			return false;
		}

		///itterates through all components on the entity and trys to cast them to the component type provided as <T> returns true and modifies outComponent input if found, else returns false
		//CURRENTLY NOT ABLE TO STORE THE POINTER IN A REFERENCE
		template<typename T>
		bool TryGetComponent(T& outComponent)
		{
			LOGCORE_ERROR("Attempting to pass a reference into the outComponent of TryGetComponent, This is not yet supported! Please use a raw pointer");
			if (std::is_base_of<Component, T>::value)
			{
				std::vector<Component*> components = ECSManager::GetComponentsFromIDs(ECSManager::GetComponentsOnEntity(m_entityID));
				for (Component* c : components)
				{
					T* component = dynamic_cast<T*>(c); //this will return a nullptr if the cast was not possible
					if (component != nullptr)
					{
						outComponent = *component;
						return true;
					}
				}
				LOGCORE_WARN("Component could not be found on Entity: {1} ID: {2}", m_name, m_entityID);
				return false;
			}
			LOGCORE_WARN("Component given to template TryGetComponent() is not derived from type component");
			return false;
		}

		template<typename T>
		bool HasComponent()
		{
			if (std::is_base_of<Component, T>::value)
			{
				std::vector<Component*> components = ECSManager::GetComponentsFromIDs(ECSManager::GetComponentsOnEntity(m_entityID));
				for (Component* c : components)
				{
					T* component = dynamic_cast<T*>(c); //this will return a nullptr if the cast was not possible
					if (component != nullptr)
					{
						return true;
					}
				}
				LOGCORE_WARN("Component could not be found on Entity: {1} ID: {2}", m_name, m_entityID);
				return false;
			}
			LOGCORE_WARN("Component given to template TryGetComponent() is not derived from type component");
			return false;
		}

		char* GetName() { return m_name; }
		size_t GetEntityID() { return m_entityID; }
		std::vector<Component*> GetAllComponents() { return ECSManager::GetComponentsFromIDs(ECSManager::GetComponentsOnEntity(m_entityID));; }
		Transform* GetTransform() { return m_transform; }

	private:
		uint32_t m_entityID;
		char m_name[MAX_ENTITY_NAME_LENGTH];
		Transform* m_transform = new Transform();
	};
}