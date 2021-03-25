#include "chaospch.h"

#include "colliders.h"
#include "Chaos/Maths/Collisions.h"
#include "Chaos/DataTypes/QuadTree.h"

// debug
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

const float DEBUG_LINE_WEIGHT = 0.02f;

namespace Chaos
{
	Collider::Collider(bool child) : Node(child), Type(ColliderType::NONE)
	{
		Name = "Collider";
	}
	
	
	BoxCollider2D::BoxCollider2D(bool child) : Collider(child)
	{
		Name = "BoxCollider2D";
		Type = ColliderType::BOX2D;
	}

	void BoxCollider2D::Debug()
	{
		Renderer& renderer = Application::Get().GetRenderer();

		Vec2 pos = GetPosition();

		Vec2 topLeft = Vec2(pos.X - Bounds.X, pos.Y + Bounds.Y);
		Vec2 topRight = Vec2(pos.X + Bounds.X , pos.Y + Bounds.Y );
		Vec2 bottomRight = Vec2(pos.X + Bounds.X , pos.Y - Bounds.Y );
		Vec2 bottomLeft = Vec2(pos.X - Bounds.X , pos.Y - Bounds.Y );

		renderer.DrawLine(topLeft, topRight, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, 1000.f);
		renderer.DrawLine(topRight, bottomRight, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, 1000.f);
		renderer.DrawLine(bottomRight, bottomLeft, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, 1000.f);
		renderer.DrawLine(bottomLeft, topLeft, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, 1000.f);
	}
	
	
	bool BoxCollider2D::CollideWith(Collider* other)
	{
		LOGCORE_WARN("BoxCollider::CollideWith not implemented!");
		return false;
	}
	
	
	void BoxCollider2D::CheckCollisions(QuadTree* tree)
	{
		Collider** nodesInRange = (Collider**)malloc(MAX_COLLIDER_OVERLAPS * sizeof(Collider*));
		size_t nodesInRangesize = 0;
		tree->QueryRange(GetPosition(), Bounds, nodesInRange, nodesInRangesize);
		
		// create array for stuff we actually hit after getting the nodes in range of our bounds
		Collider** hitNodes = (Collider**)malloc(MAX_COLLIDER_OVERLAPS * sizeof(Collider*));
		size_t insert = 0;
		for (size_t i = 0; i < nodesInRangesize; ++i)
		{
			if (!HASBIT(CollisionMask, nodesInRange[i]->ObjectMask) || nodesInRange[i] == this || nodesInRange[i]->ID == ID)
				continue;
			
			hitNodes[insert] = nodesInRange[i];
			++insert;
		}
		
		Level* level = Level::Get();
		Node* root = level->Nodes[ID][0];
		
		// determine which nodes were just hit, which have stayed, and which have left
		// collider stay and enter
		for (size_t i = 0; i < insert; ++i)
		{			
			for (size_t j = 0; j < OverlapsSize; ++j)
			{
				if (hitNodes[i] == Overlaps[j])
				{
					hitNodes[i]->Stay = true;
					
					for (size_t j = 0; j < root->ChildCount; ++j)
					{
						if (hitNodes[i]->Trigger)
							level->Nodes[ID][j]->TriggerStay(this, hitNodes[i]);
						else
							level->Nodes[ID][j]->ColliderStay(this, hitNodes[i]);
					}
				}
			}
			if (!hitNodes[i]->Stay)
			{
				for (size_t j = 0; j < root->ChildCount; ++j)
				{
					if (hitNodes[i]->Trigger)
						level->Nodes[ID][j]->TriggerEnter(this, hitNodes[i]);
					else
						level->Nodes[ID][j]->ColliderEnter(this, hitNodes[i]);
				}
			}
		}
		// colliders leaving
		for (size_t i = 0; i < OverlapsSize; ++i)
		{
			// iterate over overlaps and compare to see if something has left
			
			if (!Overlaps[i]->Stay)
			{
				for (size_t j = 0; j < root->ChildCount; ++j)
				{
					if (Overlaps[i]->Trigger)
						level->Nodes[ID][j]->TriggerExit(this, Overlaps[i]);
					else
						level->Nodes[ID][j]->ColliderExit(this, Overlaps[i]);
				}
			}
			
		}
		
		// set overlaps to the current hit nodes
		// may need to zero overlaps first here, but as we won't be accessing
		// past the size index here there's no need
		OverlapsSize = insert;
		memcpy(Overlaps, hitNodes, insert * sizeof(Collider*));
		
		// reset stay flags after we're done so the next check func can use them
		for (size_t i = 0; i < OverlapsSize; ++i)
		{
			Overlaps[i]->Stay = false;
		}
		
		free(hitNodes);
		free(nodesInRange);
	}
	
	
	CircleCollider::CircleCollider(bool child) : Collider(child)
	{
		Name = "CircleCollider";
		Type = ColliderType::CIRCLE;
	}

	void CircleCollider::Debug()
	{
		// not really optimal, but these are only for debugging so it shouldn't matter too much
		Renderer& renderer = Application::Get().GetRenderer();

		Vec2 pos = GetPosition();
		int steps = 60;

		float stepIncrement = (2 * PI) / steps;

		for (int i = 0; i < steps; ++i)
		{
			Vec2 start = Vec2(pos.X + Radius * cos(i * stepIncrement), pos.Y + Radius * sin(i * stepIncrement));
			Vec2 end = Vec2(pos.X + Radius * cos((i + 1) * stepIncrement), pos.Y + Radius * sin((i + 1) * stepIncrement));

			renderer.DrawLine(start, end, Vec4(0.3f, 1.0f, 0.3f, 0.8f), DEBUG_LINE_WEIGHT, 1000.f);
		}
	}
	
	
	bool CircleCollider::CollideWith(Collider* other)
	{
		LOGCORE_WARN("CircleCollider::CollideWith not implemented!");
		return false;
	}
	
	
	void CircleCollider::CheckCollisions(QuadTree* tree)
	{
		Collider** nodesInRange = (Collider**)malloc(MAX_COLLIDER_OVERLAPS * sizeof(Collider*));
		size_t nodesInRangesize = 0;
		tree->QueryRadius(GetPosition(), Radius, nodesInRange, nodesInRangesize);
		
		// create array for stuff we actually hit after getting the nodes in range of our bounds
		Collider** hitNodes = (Collider**)malloc(MAX_COLLIDER_OVERLAPS * sizeof(Collider*));
		size_t insert = 0;
		for (size_t i = 0; i < nodesInRangesize; ++i)
		{
			if (!HASBIT(CollisionMask, nodesInRange[i]->ObjectMask) || nodesInRange[i] == this || nodesInRange[i]->ID == ID)
				continue;
			
			hitNodes[insert] = nodesInRange[i];
			++insert;
		}
		
		Level* level = Level::Get();
		Node* root = level->Nodes[ID][0];
		
		// determine which nodes were just hit, which have stayed, and which have left
		// collider stay and enter
		for (size_t i = 0; i < insert; ++i)
		{
			for (size_t j = 0; j < OverlapsSize; ++j)
			{
				if (hitNodes[i] == Overlaps[j])
				{
					hitNodes[i]->Stay = true;
					
					for (size_t j = 0; j < root->ChildCount; ++j)
					{
						if (hitNodes[i]->Trigger)
							level->Nodes[ID][j]->TriggerStay(this, hitNodes[i]);
						else
							level->Nodes[ID][j]->ColliderStay(this, hitNodes[i]);
					}
				}
			}
			if (!hitNodes[i]->Stay)
			{
				for (size_t j = 0; j < root->ChildCount; ++j)
				{
					if (hitNodes[i]->Trigger)
						level->Nodes[ID][j]->TriggerEnter(this, hitNodes[i]);
					else
						level->Nodes[ID][j]->ColliderEnter(this, hitNodes[i]);
				}
			}
		}
		// colliders leaving
		for (size_t i = 0; i < OverlapsSize; ++i)
		{
			// iterate over overlaps and compare to see if something has left
			
			if (!Overlaps[i]->Stay)
			{
				for (size_t j = 0; j < root->ChildCount; ++j)
				{
					if (Overlaps[i]->Trigger)
						level->Nodes[ID][j]->TriggerExit(this, Overlaps[i]);
					else
						level->Nodes[ID][j]->ColliderExit(this, Overlaps[i]);
				}
			}			
		}
		
		// set overlaps to the current hit nodes
		// may need to zero overlaps first here, but as we won't be accessing
		// past the size index here there's no need
		memset(Overlaps, 0, sizeof(Overlaps));

		OverlapsSize = insert;
		memcpy(Overlaps, hitNodes, insert * sizeof(Collider*));
		
		// reset stay flags after we're done so the next check func can use them
		for (size_t i = 0; i < OverlapsSize; ++i)
		{
			Overlaps[i]->Stay = false;
		}
		
		free(hitNodes);
		free(nodesInRange);
	}
}