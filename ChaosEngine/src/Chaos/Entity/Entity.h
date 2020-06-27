#pragma once
#include "chaospch.h"
#include "Component.h"

namespace Chaos
{
	class Entity
	{
	public:
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
			}
		}

		//Checks if the given class is derived from component, if so then it initalises a new class of that type and adds it to the vector, else print a warning to console
		template<typename T>
		void AddComponent()
		{
			if (std::is_base_of<Component, T>::value)
			{
				mComponents.push_back(CreateRef<T>());
			}
			else
			{
				LOGCORE_WARN("Tried to add a component that was not derived from Component");
			}
		}

		template<typename T>
		Ref<T> GetComponent()
		{
			for (auto& c : mComponents)
			{
				Ref<T> component = std::dynamic_pointer_cast<T>(c);
				if (component != nullptr)
				{					 
					return *component.get();
				}
			}
			LOGCORE_ERROR("Failed to get component on entity {0} ID: {1} returning nullptr!", mName, mEntityID);
			return nullptr;
		}

		///itterates through all components on the entity and trys to cast them to the component type provided as <T> returns true and modifies outComponent input if found, else returns false
		template<typename T>
		bool TryGetComponent(T& outComponent)
		{
			if (std::is_base_of<Component, T>::value)
			{
				for (auto& c : mComponents)
				{
					Ref<T> component = std::dynamic_pointer_cast<T>(c);	//this will return a nullptr if the cast was not possible
					if (component != nullptr)
					{
						outComponent = *component.get();
						return true;
					}
				}
				LOGCORE_WARN("Component could not be found on Entity: {1} ID: {2}", mName, mEntityID);
				return false;
			}
			LOGCORE_WARN("Component given to template TryGetComponent() is not derived from type component");
			return false;
		}

		const char* GetName() {};
		size_t GetEntityID() {};

	private:
		size_t mEntityID;
		const char* mName;
		std::vector<Ref<Component>> mComponents;
	};
}