#pragma once
#include "Chaos/Entity/Component.h"
#include "Chaos/DataTypes/Vec3.h"
namespace Chaos
{
	enum class ColliderType
	{
		BOX2D			= 0,
		LINE
	};

	class Collider : public Component
	{
	public:
		virtual bool CollideWith(Collider& other) = 0;

		bool IsTrigger() { return m_trigger; }
		void SetTriggerActive(bool state) { m_trigger = state; }

		Vec3& Offset() { return m_offset; }

		std::vector<Collider*>& GetOverlaps() { return m_overlappedColliders; }
		ColliderType GetType() { return m_type; }

	protected:
		ColliderType m_type;
		bool m_trigger = false;
		Vec3 m_offset = Vec3::Zero();
		std::vector<Collider*> m_overlappedColliders;
	};
}