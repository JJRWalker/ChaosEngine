#include "chaospch.h"

#include "QuadTree.h"
#include "Chaos/Nodes/Colliders.h"
#include "Chaos/Maths/Collisions.h"

//#ifdef CHAOS_DEBUG
#include "Chaos/Core/Application.h"
#include "Chaos/Debug/Debug.h"
//#endif

namespace Chaos
{
	QuadTree::QuadTree(Vec2 origin, Vec2 bounds) : m_origin(origin), m_bounds(bounds)
	{
	}
	
	QuadTree::~QuadTree()
	{
		// if one is valid, all are valid 
		if (m_children.NorthEast)
		{
			delete m_children.NorthEast;
			delete m_children.NorthWest;
			delete m_children.SouthEast;
			delete m_children.SouthWest;
		}
	}
	
	bool QuadTree::Insert(Collider* node)
	{
		if (!Collisions::PointInRectangle(node->GetWorldPosition(),m_origin, m_bounds))
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
			
			return m_children.NorthWest->Insert(node) 
				|| m_children.NorthEast->Insert(node)
				|| m_children.SouthWest->Insert(node)
				|| m_children.SouthEast->Insert(node);
		}
	}
	
	Collider* QuadTree::QueryRange(Vec2 centre, Vec2 bounds, Collider* foundNodes[MAX_NODES], size_t& insert)
	{
		if (!Collisions::BoxBoxIntersection(centre, bounds, m_origin, m_bounds))
			return false;
		for (int i = 0; i < m_size; ++i)
		{
			switch (m_nodes[i]->ColliderType)
			{
				case EColliderType::BOX2D:
				{
					BoxCollider2D* collider = dynamic_cast<BoxCollider2D*>(m_nodes[i]);
					if(collider)
					{
						if (Collisions::BoxBoxIntersection(centre, bounds, collider->GetWorldPosition(), collider->Bounds))
						{
							foundNodes[insert] = collider;
							++insert;
						}
					}
				}break;
				case EColliderType::CIRCLE:
				{
					CircleCollider* collider = dynamic_cast<CircleCollider*>(m_nodes[i]);
					if (collider)
					{
						if (Collisions::CircleBoxIntersection(collider->GetWorldPosition(), collider->Radius, centre, bounds))
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
			switch (m_nodes[i]->ColliderType)
			{
				case EColliderType::BOX2D:
				{
					BoxCollider2D* collider = dynamic_cast<BoxCollider2D*>(m_nodes[i]);
					if (collider)
					{
						if (Collisions::CircleBoxIntersection(centre, radius, collider->GetWorldPosition(), collider->Bounds))
						{
							foundNodes[insert] = collider;
							++insert;
						}
					}
				}break;
				case EColliderType::CIRCLE:
				{
					CircleCollider* collider = dynamic_cast<CircleCollider*>(m_nodes[i]);
					if (collider)
					{
						if (Collisions::CircleCircleIntersection(collider->GetWorldPosition(), collider->Radius, centre, radius))
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
	
	
	Collider* QuadTree::QueryLine(Vec2 start, Vec2 end, Collider* foundNodes[MAX_NODES], size_t& insert)
	{
		//TODO EFFICENCY: find way to discriminate based on line intersection.
		//if (!Collisions::BoxBoxIntersection(centre, bounds, m_origin, m_bounds))
		//return false;
		
		for (int i = 0; i < m_size; ++i)
		{
			switch (m_nodes[i]->ColliderType)
			{
				case EColliderType::BOX2D:
				{
					BoxCollider2D* collider = dynamic_cast<BoxCollider2D*>(m_nodes[i]);
					if (collider)
					{
						if (Collisions::LineBoxIntersection(start, end, collider->GetWorldPosition(), collider->Bounds))
						{
							foundNodes[insert] = collider;
							++insert;
						}
					}
				}break;
				case EColliderType::CIRCLE:
				{
					CircleCollider* collider = dynamic_cast<CircleCollider*>(m_nodes[i]);
					if (collider)
					{
						if (Collisions::LineCircleIntersection(start, end, collider->GetWorldPosition(), collider->Radius))
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
			m_children.NorthWest->QueryLine(start, end, foundNodes, insert);
			m_children.NorthEast->QueryLine(start, end, foundNodes, insert);
			m_children.SouthWest->QueryLine(start, end, foundNodes, insert);
			m_children.SouthEast->QueryLine(start, end, foundNodes, insert);
		}
		
		return foundNodes[0];
	}
	
	
	void QuadTree::Debug()
	{
		Vec2 bottomLeft = Vec2(m_origin.X - m_bounds.X, m_origin.Y - m_bounds.Y);
		Vec2 bottomRight = Vec2(m_origin.X + m_bounds.X, m_origin.Y - m_bounds.Y);
		Vec2 topLeft = Vec2(m_origin.X - m_bounds.X, m_origin.Y + m_bounds.Y);
		Vec2 topRight = Vec2(m_origin.X + m_bounds.X, m_origin.Y + m_bounds.Y);
		
		Debug::DrawLine(bottomLeft, bottomRight, Vec4(1.0f, 0.1f, 1.0f, 1.0f), 0.1f);
		Debug::DrawLine(bottomRight, topRight, Vec4(1.0f, 0.1f, 1.0f, 1.0f), 0.1f);
		Debug::DrawLine(topRight, topLeft, Vec4(1.0f, 0.1f, 1.0f, 1.0f), 0.1f);
		Debug::DrawLine(topLeft, bottomLeft, Vec4(1.0f, 0.1f, 1.0f, 1.0f), 0.1f);
		
		if (m_divided)
		{
			m_children.NorthWest->Debug();
			m_children.NorthEast->Debug();
			m_children.SouthWest->Debug();
			m_children.SouthEast->Debug();
		}
	}
	
	
	size_t QuadTree::Size()
	{
		size_t size = QUAD_TREE_CAPACITY;
		if (m_divided)
		{
			size += m_children.NorthWest->Size();
			size += m_children.NorthEast->Size();
			size += m_children.SouthWest->Size();
			size += m_children.SouthEast->Size();
		}
		
		return size;
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