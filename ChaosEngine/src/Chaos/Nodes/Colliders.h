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
	

	struct CollisionInfo
	{
		Collider* pCollider = nullptr;
		bool Stay = false;
	};
	
	class Collider : public Node
	{
		public:
		Collider(bool child = false);
		
		virtual bool CollideWith(Collider* other) = 0;
		// checks all collisions with quad tree, searches based on the type of colliders involved
		virtual void CheckCollisions(QuadTree* tree) = 0;
		virtual void CheckCollisionExit();
		virtual void InsertOverlap(Collider* collider);
		virtual void InsertOverlaps(Collider** colliders, size_t size);
		virtual void RemoveOverlap(Collider* collider);
		virtual bool OverlapsWith(Collider* other);
		virtual void ClearOverlaps();
		virtual void InsertHitNode(Collider* collider);

		public:
		bool Trigger = false;
		ColliderType Type;
		PhysicsType PhysicsUpdateType = PhysicsType::DISCREET;
		uint32_t CollisionMask = 1; // bitmask for what it objectmasks it will collide with
		uint32_t ObjectMask = 1;  // seperate bitmask for the type it is
		Collider* Overlaps[MAX_COLLIDER_OVERLAPS];
		size_t OverlapsSize = 0;

		protected:
		Collider* m_hitNodes[MAX_COLLIDER_OVERLAPS];
		size_t m_hitNodesSize = 0;
	};
	
	
	class BoxCollider2D : public Collider
	{
		public:
		BoxCollider2D(bool child = false);
		
		void Debug() override;

		bool CollideWith(Collider* other) override;
		void CheckCollisions(QuadTree* tree) override;
		
		Vec2 Bounds = Vec2(0.5f, 0.5f);
	};
	
	
	class CircleCollider : public Collider
	{
		public:
		CircleCollider(bool child = false);

		void Debug() override;
		
		bool CollideWith(Collider* other) override;
		void CheckCollisions(QuadTree* tree) override;
		
		public:
		float Radius = 1.0f;
	};
}

#endif //_COLLIDERS_H
