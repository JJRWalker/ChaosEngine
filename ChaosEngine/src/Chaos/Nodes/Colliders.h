/* date = January 31st 2021 2:07 pm */

#ifndef _COLLIDERS_H
#define _COLLIDERS_H

#include "Chaos/Nodes/Node.h"

namespace Chaos
{
	
	const uint32_t MAX_COLLIDER_OVERLAPS = 1000;
	
	class QuadTree;
	
	enum class ColliderType
	{
		NONE = 0,  // shouldn't really ever be none, but good to have a value for uninitted
		BOX2D = 1,
		CIRCLE = 2
	};
	
	
	enum class PhysicsType
	{
		DISCREET = 0,
		CONTINUOUS = 1
	};
	
	
	class Collider : public Node
	{
		public:
		Collider();
		
		virtual bool CollideWith(Collider* other) = 0;
		// checks all collisions with quad tree, searches based on the type of colliders involved
		virtual void CheckCollisions(QuadTree* tree) = 0;
		
		bool Trigger = false;
		ColliderType Type;
		PhysicsType PhysicsUpdateType = PhysicsType::DISCREET;
		uint32_t CollisionMask = 1; // bitmask for what it objectmasks it will collide with
		uint32_t ObjectMask = 1;  // seperate bitmask for the type it is
		Collider* Overlaps[MAX_COLLIDER_OVERLAPS];
		size_t OverlapsSize = 0;
		
		// flag used for determining the collision funcs to call
		bool Stay = false;
	};
	
	
	class BoxCollider2D : public Collider
	{
		public:
		BoxCollider2D();
		
		bool CollideWith(Collider* other) override;
		void CheckCollisions(QuadTree* tree) override;
		
		Vec2 Bounds = Vec2(2.0f, 2.0f);
	};
	
	
	class CircleCollider : public Collider
	{
		public:
		CircleCollider();
		
		bool CollideWith(Collider* other) override;
		void CheckCollisions(QuadTree* tree) override;
		
		public:
		float Radius = 2.0f;
	};
}

#endif //_COLLIDERS_H
