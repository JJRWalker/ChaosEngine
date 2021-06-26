#include "chaospch.h"

#include "colliders.h"
#include "Chaos/Maths/Collisions.h"
#include "Chaos/DataTypes/QuadTree.h"

// debug
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

const float DEBUG_LINE_WEIGHT = 0.025f;
const float DEBUG_RENDER_ORDER = 1.0f;

namespace Chaos
{
	Collider::Collider(bool child) : Node(child), Type(ColliderType::NONE)
	{
		Name = "Collider";
	}
	
	
	void Collider::CheckCollisionExit()
	{
		Level* level = Level::Get();
		Node* root = level->Nodes[ID][0];
		// colliders leaving
		for (size_t i = 0; i < OverlapsSize; ++i)
		{
			// iterate over overlaps and compare to see if something has left
			bool foundHitNodeInOverlaps = false;
			for (size_t j = 0; j < m_hitNodesSize; ++j)
			{
				if (Overlaps[i] == m_hitNodes[j])
				{
					foundHitNodeInOverlaps = true;
					break;
				}
			}
			
			if (!foundHitNodeInOverlaps)
			{
				for (size_t j = 0; j < root->ChildCount; ++j)
				{
					if (Overlaps[i]->Trigger)
						level->Nodes[ID][j]->TriggerExit(this, Overlaps[i]);
					else
						level->Nodes[ID][j]->ColliderExit(this, Overlaps[i]);
				}
				
				RemoveOverlap(Overlaps[i]);
				--i;
			}
		}
	}
	
	
	// inserts single node into overlaps, calls enter and stay methods for nodes.
	void Collider::InsertOverlap(Collider* collider)
	{
		Level* level = Level::Get();
		Node* root = level->Nodes[ID][0];
		
		bool alreadyInOvelaps = false;
		for (size_t j = 0; j < OverlapsSize; ++j)
		{
			if (collider == Overlaps[j])
			{
				alreadyInOvelaps = true;
				
				for (size_t j = 0; j < root->ChildCount; ++j)
				{
					if (collider->Trigger)
						level->Nodes[ID][j]->TriggerStay(this, collider);
					else
						level->Nodes[ID][j]->ColliderStay(this, collider);
				}
			}
		}
		if (!alreadyInOvelaps)
		{
			for (size_t j = 0; j < root->ChildCount; ++j)
			{
				if (collider->Trigger)
					level->Nodes[ID][j]->TriggerEnter(this, collider);
				else
					level->Nodes[ID][j]->ColliderEnter(this, collider);
			}
			Overlaps[OverlapsSize] = collider;
			++OverlapsSize;
		}
	}
	
	
	// inserts nodes into overlaps, calls enter and stay methods for nodes.
	void Collider::InsertOverlaps(Collider** colliders, size_t size)
	{
		Level* level = Level::Get();
		Node* root = level->Nodes[ID][0];
		
		// determine which nodes were just hit, which have stayed, and which have left
		// collider stay and enter
		for (size_t i = 0; i < size; ++i)
		{
			bool alreadyInOvelaps = false;
			for (size_t j = 0; j < OverlapsSize; ++j)
			{
				if (colliders[i] == Overlaps[j])
				{
					alreadyInOvelaps = true;
					
					for (size_t j = 0; j < root->ChildCount; ++j)
					{
						if (colliders[i]->Trigger)
							level->Nodes[ID][j]->TriggerStay(this, colliders[i]);
						else
							level->Nodes[ID][j]->ColliderStay(this, colliders[i]);
					}
				}
			}
			if (!alreadyInOvelaps)
			{
				for (size_t j = 0; j < root->ChildCount; ++j)
				{
					if (colliders[i]->Trigger)
						level->Nodes[ID][j]->TriggerEnter(this, colliders[i]);
					else
						level->Nodes[ID][j]->ColliderEnter(this, colliders[i]);
				}
				Overlaps[OverlapsSize] = colliders[i];
				++OverlapsSize;
			}
		}
	}
	
	
	void Chaos::Collider::RemoveOverlap(Collider* collider)
	{
		for (size_t i = 0; i < OverlapsSize; ++i)
		{
			if (Overlaps[i] == collider)
			{
				// move to be removed to the end of the array
				for (size_t j = i; j < OverlapsSize - 1; ++j)
				{
					Collider* swapTemp = Overlaps[j];
					Overlaps[j] = Overlaps[j + 1];
					Overlaps[j + 1] = swapTemp;
				}
				
				Overlaps[OverlapsSize - 1] = nullptr;
				--OverlapsSize;
				return;
			}
		}
	}
	
	
	bool Collider::OverlapsWith(Collider* other)
	{
		for (int i = 0; i < OverlapsSize; ++i)
		{
			if (Overlaps[i] == other)
			{
				return true;
			}
		}
		
		return false;
	}
	
	
	void Collider::ClearOverlaps()
	{
		memset(Overlaps, 0, sizeof(Overlaps));
		
		OverlapsSize = 0;
	}
	
	
	void Chaos::Collider::InsertHitNode(Collider* collider)
	{
		m_hitNodes[m_hitNodesSize] = collider;
		++m_hitNodesSize;
	}
	
	
	BoxCollider2D::BoxCollider2D(bool child) : Collider(child)
	{
		Name = "BoxCollider2D";
		Type = ColliderType::BOX2D;
	}
	
	void BoxCollider2D::OnDebug()
	{
		Renderer& renderer = Application::Get().GetRenderer();
		
		Vec2 pos = GetWorldPosition();
		
		Vec2 topLeft = Vec2(pos.X - Bounds.X, pos.Y + Bounds.Y);
		Vec2 topRight = Vec2(pos.X + Bounds.X , pos.Y + Bounds.Y );
		Vec2 bottomRight = Vec2(pos.X + Bounds.X , pos.Y - Bounds.Y );
		Vec2 bottomLeft = Vec2(pos.X - Bounds.X , pos.Y - Bounds.Y );
		
		renderer.DrawLine(topLeft, topRight, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, DEBUG_RENDER_ORDER);
		renderer.DrawLine(topRight, bottomRight, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, DEBUG_RENDER_ORDER);
		renderer.DrawLine(bottomRight, bottomLeft, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, DEBUG_RENDER_ORDER);
		renderer.DrawLine(bottomLeft, topLeft, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, DEBUG_RENDER_ORDER);
	}
	
	
	bool BoxCollider2D::CollideWith(Collider* other)
	{
		LOGCORE_WARN("BoxCollider::CollideWith not implemented!");
		return false;
	}
	
	
	void BoxCollider2D::CheckCollisions(QuadTree* tree)
	{
		// clear hit nodes (should only contain nodes from last check this object did
		memset(m_hitNodes, 0, sizeof(m_hitNodes));
		m_hitNodesSize = 0;
		
		Collider** nodesInRange = (Collider**)malloc(MAX_COLLIDER_OVERLAPS * sizeof(Collider*));
		size_t nodesInRangesize = 0;
		tree->QueryRange(GetWorldPosition(), Bounds, nodesInRange, nodesInRangesize);
		
		for (size_t i = 0; i < nodesInRangesize; ++i)
		{
			if (!HASBIT(CollisionMask, nodesInRange[i]->ObjectMask) || nodesInRange[i] == this || nodesInRange[i]->ID == ID)
				continue;
			
			m_hitNodes[m_hitNodesSize] = nodesInRange[i];
			++m_hitNodesSize;
		}
		
		InsertOverlaps(m_hitNodes, m_hitNodesSize);
		
		for (size_t i = 0; i < m_hitNodesSize; ++i)
		{
			m_hitNodes[i]->InsertOverlap(this);
			m_hitNodes[i]->InsertHitNode(this);
		}
		
		free(nodesInRange);
	}
	
	
	CircleCollider::CircleCollider(bool child) : Collider(child)
	{
		Name = "CircleCollider";
		Type = ColliderType::CIRCLE;
	}
	
	void CircleCollider::OnDebug()
	{
		// not really optimal, but these are only for debugging so it shouldn't matter too much
		Renderer& renderer = Application::Get().GetRenderer();
		
		Vec2 pos = GetWorldPosition();
		int steps = 60;
		
		float stepIncrement = (2 * PI) / steps;
		
		for (int i = 0; i < steps; ++i)
		{
			Vec2 start = Vec2(pos.X + Radius * cos(i * stepIncrement), pos.Y + Radius * sin(i * stepIncrement));
			Vec2 end = Vec2(pos.X + Radius * cos((i + 1) * stepIncrement), pos.Y + Radius * sin((i + 1) * stepIncrement));
			
			renderer.DrawLine(start, end, Vec4(0.3f, 1.0f, 0.3f, 0.8f), DEBUG_LINE_WEIGHT, DEBUG_RENDER_ORDER);
		}
	}
	
	
	bool CircleCollider::CollideWith(Collider* other)
	{
		LOGCORE_WARN("CircleCollider::CollideWith not implemented!");
		return false;
	}
	
	
	void CircleCollider::CheckCollisions(QuadTree* tree)
	{
		// clear hit nodes(should only contain nodes hit this collision check)
		memset(m_hitNodes, 0, sizeof(m_hitNodes));
		m_hitNodesSize = 0;
		
		Collider** nodesInRange = (Collider**)malloc(MAX_COLLIDER_OVERLAPS * sizeof(Collider*));
		size_t nodesInRangesize = 0;
		tree->QueryRadius(GetWorldPosition(), Radius, nodesInRange, nodesInRangesize);
		
		for (size_t i = 0; i < nodesInRangesize; ++i)
		{
			if (!HASBIT(CollisionMask, nodesInRange[i]->ObjectMask) || nodesInRange[i] == this || nodesInRange[i]->ID == ID)
				continue;
			
			m_hitNodes[m_hitNodesSize] = nodesInRange[i];
			++m_hitNodesSize;
		}
		
		InsertOverlaps(m_hitNodes, m_hitNodesSize);
		
		for (size_t i = 0; i < m_hitNodesSize; ++i)
		{
			m_hitNodes[i]->InsertOverlap(this);
			m_hitNodes[i]->InsertHitNode(this);
		}
		
		free(nodesInRange);
	}
}