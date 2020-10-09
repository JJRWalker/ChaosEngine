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
		Vec2 GetExtents() { return m_extents; }
		void SetBounds(Vec2 value) { m_bounds = value; m_extents = value / 2;}
		
		void Debug() override;
		
		private:
		Vec2 m_bounds = Vec2(1.0f, 1.0f); //total width and height of the collider
		Vec2 m_extents = Vec2(0.5f, 0.5f); //extents are just half bounds, stored to save on calculations later
		
	};
}