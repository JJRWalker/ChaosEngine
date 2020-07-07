#pragma once
#include "chaospch.h"
#include "Component.h"
#include "Components/Transform.h"	//Only components included are those that do not have a reference to their owner

namespace Chaos
{
	//static count of all entities, incremented each time an entity is created. Can be used to limit how many entities can be created in the future
	static size_t sEntityCount = 0;
	static const size_t MAX_ENTITY_NAME_LENGTH = 128;

	class Entity
	{
	public:
		Entity() : m_name("entity"), m_entityID(sEntityCount) { ++sEntityCount; }
		Entity(char* name) : m_entityID(sEntityCount) { ++sEntityCount; strcpy_s(m_name, name); };
		~Entity()
		{
			Destroy();
			m_components.clear();
		}

		void Start() 
		{
			for (auto& c : m_components)
			{
				c->Start();
			}
		}
		void Update() 
		{
			for (auto& c : m_components)
			{
				c->Update();
			}
		}
		void Destroy()
		{
			for (auto& c : m_components)
			{
				c->Destroy();
			}
		}

		//Checks if the given class is derived from component, if so then it initalises a new class of that type and adds it to the vector, else print a warning to console
		template<typename T>
		void AddComponent()
		{
			if (std::is_base_of<IComponent, T>::value)
			{
				T* component = new T(this);
				m_components.push_back(component);
			}
			else
			{
				LOGCORE_WARN("Tried to add a component that was not derived from Component");
			}
		}

		template<typename T>
		T* GetComponent()
		{
			for (IComponent* c : m_components)
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
			if (std::is_base_of<IComponent, T>::value)
			{
				for (IComponent* c : m_components)
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
			if (std::is_base_of<IComponent, T>::value)
			{
				for (IComponent* c : m_components)
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
			if (std::is_base_of<IComponent, T>::value)
			{
				for (IComponent* c : m_components)
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
		std::vector<IComponent*>& GetAllComponents() { return m_components; }
		Transform* GetTransform() { return m_transform; }

	private:
		size_t m_entityID;
		char m_name[MAX_ENTITY_NAME_LENGTH];
		Transform* m_transform = new Transform();	
		std::vector<IComponent*> m_components;
	};
}