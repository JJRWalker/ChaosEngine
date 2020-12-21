#pragma once
#include "Chaos/Entity/Component.h"
#include "Chaos/DataTypes/Vec3.h"
namespace Chaos
{
	enum class ColliderType
	{
		BOX2D			= 0,
		LINE,
		CIRCLE
	};
	
	class Collider : public Component
	{
		public:
		virtual bool CollideWith(Collider& other) = 0;
		
		bool IsTrigger() { return m_trigger; }
		void SetTrigger(bool state) { m_trigger = state; }
		
		Vec3& Offset() { return m_offset; }
		Vec3& GetPosition() override {
			m_position = GetEntity()->GetPosition() + m_offset;
			return m_position;
		}
		
		std::vector<Collider*>& GetOverlaps() { return m_overlappedColliders; }
		ColliderType GetType() { return m_type; }
		
		public:
		//collision bit mask, values for which will be defined externally per project
		uint32_t CollisionMask = 0;
		
		protected:
		ColliderType m_type;
		bool m_trigger = false;
		Vec3 m_offset = Vec3::Zero();
		Vec3 m_position;
		std::vector<Collider*> m_overlappedColliders;
	};
}