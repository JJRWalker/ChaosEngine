#pragma once
#include "chaospch.h"
#include "ECSManager.h"

namespace Chaos
{
#define COMPONENT() virtual const char* ToString() override { return typeid(*this).name();}

	class Entity;
	class Component
	{
		friend class ECSManager;
	public:
		virtual void Start() {};
		virtual void Update() {};
		virtual void Destroy() {};

		Entity* GetEntity() { return ECSManager::GetEntityFromComponent(m_componentID); }
		uint32_t GetComponentID() { return m_componentID; }
		virtual const char* ToString() { return typeid(*this).name(); }

	private:
		uint32_t m_componentID;
	};
}