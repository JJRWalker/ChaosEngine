/* date = January 31st 2021 2:23 pm */

// for now only supports storing types of Chaos::Collider, making it type agnostic would
// probably overcomplicate things

#ifndef _QUAD_TREE_H
#define _QUAD_TREE_H

#include "Chaos/Core/Core.h"
#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class Collider;
	class QuadTree;
	
	const size_t QUAD_TREE_CAPACITY = 4;
	
	struct ChildTrees
	{
		QuadTree* NorthWest;
		QuadTree* NorthEast;
		QuadTree* SouthWest;
		QuadTree* SouthEast;
	};
	
	class QuadTree
	{
		public:
		QuadTree(Vec2 origin = Vec2(0,0), Vec2 bounds = Vec2(WORLD_SIZE, WORLD_SIZE));
		
		bool Insert(Collider* node);
		Collider* QueryRange(Vec2 centre, Vec2 bounds, Collider* foundNodes[MAX_NODES], size_t& insert);
		Collider* QueryRadius(Vec2 centre, float radius, Collider* foundNodes[MAX_NODES], size_t& insert);
		
		private:
		
		void Subdivide();
		
		private:
		Vec2 m_origin = Vec2(0, 0);
		Vec2 m_bounds;
		Collider* m_nodes[QUAD_TREE_CAPACITY];
		ChildTrees m_children;
		bool m_divided = false;
		size_t m_size = 0;
	};
}

#endif //_QUAD_TREE_H
