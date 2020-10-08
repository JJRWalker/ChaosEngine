#pragma once
#include "Chaos/Entity/Components/Collider.h"
#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class BoxCollider2D : public Collider
	{
		public:
		BoxCollider2D() { m_type = ColliderType::BOX2D; }
		
		bool CollideWith(Collider& other) override;
		bool PointCollision(Vec2& point);
		
		//specifically not returning a reference, as we only want to explicitly change bounds
		//also potentially want to do something before / after changing them so not making them public
		Vec2 GetBounds() { return m_bounds; }
		void SetBounds(Vec2 value) { m_bounds = value; }
		
		private:
		Vec2 m_bounds = Vec2(1.0f, 1.0f);
		
	};
}