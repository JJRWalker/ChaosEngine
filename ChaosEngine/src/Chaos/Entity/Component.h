#pragma once
#include "chaospch.h"
#include "ECSManager.h"
#include <Chaos/DataTypes/Vec3.h>

namespace Chaos
{
	class Entity;
	class Collider;
	class Component
	{
		friend class ECSManager;
		public:
		virtual void Start() {};
		virtual void Update() {};
		virtual void FixedUpdate() {};
		virtual void ColliderHit(Collider* self, Collider* other) {};
		virtual void TriggerHit(Collider* self, Collider* other) {};
		virtual void Destroy() {};
		virtual void Debug() {};
		
		Entity* GetEntity();
		virtual Vec3& GetPosition();
		uint32_t GetComponentID() { return m_componentID; }
		bool GetActive() {return m_active;}
		void SetActive(bool value){m_active = value;}
		virtual const char* ToString() { return typeid(*this).name(); }
		
		protected:
		uint32_t m_componentID;
		bool m_active = true;
	};
}
