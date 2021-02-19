#include "chaospch.h"

#include "QuadTree.h"
#include "Chaos/Nodes/Colliders.h"
#include "Chaos/Maths/Collisions.h"

namespace Chaos
{
	QuadTree::QuadTree(Vec2 origin, Vec2 bounds) : m_origin(origin), m_bounds(bounds)
	{
		
	}
	
	bool QuadTree::Insert(Collider* node)
	{
		if (!node->GetPosition().InsideBounds(m_origin, m_bounds))
			return false;
		
		if (m_size < QUAD_TREE_CAPACITY)
		{
			m_nodes[m_size] = node;
			++m_size;
			return true;
		}
		else
		{
			if (!m_divided)
				Subdivide();
			
			return m_children.NorthWest->Insert(node) ||
				m_children.NorthEast->Insert(node) ||
				m_children.SouthWest->Insert(node) ||
				m_children.SouthEast->Insert(node);
		}
	}
	
	Collider* QuadTree::QueryRange(Vec2 centre, Vec2 bounds, Collider* foundNodes[MAX_NODES], size_t& insert)
	{
		if (!Collisions::BoxBoxIntersection(centre, bounds, m_origin, m_bounds))
			return false;
		for (int i = 0; i < m_size; ++i)
		{
			switch (m_nodes[i]->Type)
			{
				case ColliderType::BOX2D:
				{
					BoxCollider2D* collider = dynamic_cast<BoxCollider2D*>(m_nodes[i]);
					if(collider)
					{
						if (Collisions::BoxBoxIntersection(centre, bounds, collider->GetPosition(), collider->Bounds))
						{
							foundNodes[insert] = collider;
							++insert;
						}
					}
				}break;
				case ColliderType::CIRCLE:
				{
					CircleCollider* collider = dynamic_cast<CircleCollider*>(m_nodes[i]);
					if (collider)
					{
						if (Collisions::CircleBoxIntersection(collider->GetPosition(), collider->Radius, centre, bounds))
						{
							foundNodes[insert] = collider;
							++insert;
						}
					}
				}break;
			}
		}
		
		if (m_divided)
		{
			m_children.NorthWest->QueryRange(centre, bounds, foundNodes, insert);
			m_children.NorthEast->QueryRange(centre, bounds, foundNodes, insert);
			m_children.SouthWest->QueryRange(centre, bounds, foundNodes, insert);
			m_children.SouthEast->QueryRange(centre, bounds, foundNodes, insert);
		}
		
		return foundNodes[0];
	}
	
	
	Collider* QuadTree::QueryRadius(Vec2 centre, float radius, Collider* foundNodes[MAX_NODES], size_t& insert)
	{
		if (!Collisions::CircleBoxIntersection(centre, radius, m_origin, m_bounds))
			return false;
		for (int i = 0; i < m_size; ++i)
		{
			switch (m_nodes[i]->Type)
			{
				case ColliderType::BOX2D:
				{
					BoxCollider2D* collider = dynamic_cast<BoxCollider2D*>(m_nodes[i]);
					if (collider)
					{
						if (Collisions::CircleBoxIntersection(centre, radius, collider->GetPosition(), collider->Bounds))
						{
							foundNodes[insert] = collider;
							++insert;
						}
					}
				}break;
				case ColliderType::CIRCLE:
				{
					CircleCollider* collider = dynamic_cast<CircleCollider*>(m_nodes[i]);
					if (collider)
					{
						if (Collisions::CircleCircleIntersection(collider->GetPosition(), collider->Radius, centre, radius))
						{
							foundNodes[insert] = collider;
							++insert;
						}
					}
				}break;
			}
		}
		
		if (m_divided)
		{
			m_children.NorthWest->QueryRadius(centre, radius, foundNodes, insert);
			m_children.NorthEast->QueryRadius(centre, radius, foundNodes, insert);
			m_children.SouthWest->QueryRadius(centre, radius, foundNodes, insert);
			m_children.SouthEast->QueryRadius(centre, radius, foundNodes, insert);
		}
		
		return foundNodes[0];
	}
	
	
	void QuadTree::Subdivide()
	{
		float halfWidth = m_bounds.X * 0.5f;
		float halfHeight = m_bounds.Y * 0.5f;
		
		m_children.NorthWest = new QuadTree(Vec2(m_origin.X - halfWidth, m_origin.Y + halfHeight), Vec2(halfWidth, halfHeight));
		m_children.NorthEast = new QuadTree(Vec2(m_origin.X + halfWidth, m_origin.Y + halfHeight), Vec2(halfWidth, halfHeight));
		m_children.SouthWest = new QuadTree(Vec2(m_origin.X - halfWidth, m_origin.Y - halfHeight), Vec2(halfWidth, halfHeight));
		m_children.SouthEast = new QuadTree(Vec2(m_origin.X + halfWidth, m_origin.Y - halfHeight), Vec2(halfWidth, halfHeight));
		
		m_divided = true;
	}
}