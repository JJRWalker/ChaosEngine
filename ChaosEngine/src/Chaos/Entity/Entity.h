#pragma once
#include "chaospch.h"
#include "Component.h"
#include "Components/Transform.h"	//Only components included are those that do not have a reference to their owner

namespace Chaos
{
	//static count of all entities, incremented each time an entity is created. Can be used to limit how many entities can be created in the future
	static size_t sEntityCount = 0;

	class Entity
	{
	public:
		Entity() : mName("entity"), mEntityID(sEntityCount) { ++sEntityCount; LOGCORE_TRACE("Entity Created with ID {0} Name {1}", mEntityID, mName); }
		Entity(const char* name) : mName(name), mEntityID(sEntityCount) { ++sEntityCount; };
		~Entity()
		{
			Destroy();
			mComponents.clear();
		}

		void Start() 
		{
			for (auto& c : mComponents)
			{
				c->Start();
			}
		}
		void Update() 
		{
			for (auto& c : mComponents)
			{
				c->Update();
			}
		}
		void Destroy()
		{
			for (auto& c : mComponents)
			{
				c->Destroy();
				delete c;
			}
		}

		//Checks if the given class is derived from component, if so then it initalises a new class of that type and adds it to the vector, else print a warning to console
		template<typename T>
		void AddComponent()
		{
			if (std::is_base_of<Component, T>::value)
			{
				T* component = new T(this);
				mComponents.push_back(component);
			}
			else
			{
				LOGCORE_WARN("Tried to add a component that was not derived from Component");
			}
		}

		template<typename T>
		T* GetComponent()
		{
			for (Component* c : mComponents)
			{
				T* component = dynamic_cast<T*>(c); //this will return a nullptr if the cast was not possible
				if (component != nullptr)
				{					 
					return component;
				}
			}
			LOGCORE_ERROR("Failed to get component on entity {0} ID: {1} returning nullptr!", mName, mEntityID);
			return nullptr;
		}

		///itterates through all components on the entity and trys to cast them to the component type provided as <T> returns true and modifies outComponent input if found, else returns false
		template<typename T>
		bool TryGetComponent(T* outComponent)
		{
			if (std::is_base_of<Component, T>::value)
			{
				for (Component* c : mComponents)
				{
					T* component = dynamic_cast<T*>(c); //this will return a nullptr if the cast was not possible
					if (component != nullptr)
					{
						outComponent = component;
						return true;
					}
				}
				LOGCORE_WARN("Component could not be found on Entity: {1} ID: {2}", mName, mEntityID);
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
				for (Component* c : mComponents)
				{
					T* component = dynamic_cast<T*>(c); //this will return a nullptr if the cast was not possible
					if (component != nullptr)
					{
						outComponent = *component;
						return true;
					}
				}
				LOGCORE_WARN("Component could not be found on Entity: {1} ID: {2}", mName, mEntityID);
				return false;
			}
			LOGCORE_WARN("Component given to template TryGetComponent() is not derived from type component");
			return false;
		}

		const char* GetName() { return mName; }
		size_t GetEntityID() { return mEntityID; }
		Ref<Transform> GetTransform() { return mTransform; }

	private:
		size_t mEntityID;
		const char* mName;
		Ref<Transform> mTransform = CreateRef<Transform>();	
		std::vector<Component*> mComponents;
	};
}