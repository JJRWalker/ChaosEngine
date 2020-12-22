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
		//bitwise checks the mask with the mask given, can be used for a form of layer system
		bool HasCollisionMask(uint32_t mask) {return (CollisionMask & mask) != 0;}
		bool HasObjectMask(uint32_t mask) {return (ObjectMask & mask) != 0;}
		
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
		//object bit mask, like collision mask, however it specifies what type of object this collider is. meaning it can be of a type but not collide with that type
		uint32_t ObjectMask = 0;
		
		protected:
		ColliderType m_type;
		bool m_trigger = false;
		Vec3 m_offset = Vec3::Zero();
		Vec3 m_position;
		std::vector<Collider*> m_overlappedColliders;
	};
}