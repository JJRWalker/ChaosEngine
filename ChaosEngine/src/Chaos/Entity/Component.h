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
		virtual void FixedUpdate() {};
		virtual void Destroy() {};
		virtual void Debug() {};
		
		Entity* GetEntity() { return ECSManager::GetEntityFromComponent(m_componentID); }
		uint32_t GetComponentID() { return m_componentID; }
		bool GetActive() {return m_active;}
		void SetActive(bool value){m_active = value;}
		virtual const char* ToString() { return typeid(*this).name(); }
		
		protected:
		uint32_t m_componentID;
		bool m_active = true;
	};
}
