#include "chaospch.h"
#include "Level.h"

#include "Chaos/Nodes/Node.h"
#include "Chaos/Nodes/Camera.h"
#include "Chaos/Nodes/Colliders.h"
#include "Chaos/DataTypes/QuadTree.h"

namespace Chaos
{
	Level* Level::s_instance = nullptr;
	
	Level::Level()
	{
	}
	
	
	void Level::Start()
	{
	}
	
	
	void Level::Update(float delta)
	{
		QuadTree quadTree; // need to reconstruct each loop
		Collider** colliders = (Collider**)malloc(MAX_NODES * sizeof(Collider*)); // need to allocate this on the heap, too much for stack
		size_t collidableCount = 0;
		for (int node = 0; node < NodeCount; ++node)
		{
			for (int child = 0; child <= Nodes[node][0]->ChildCount; ++child)
			{
				Nodes[node][child]->Update(delta);
				Collider* collider = dynamic_cast<Collider*>(Nodes[node][child]);
				if (collider)
				{
					if (collider->PhysicsUpdateType == PhysicsType::CONTINUOUS)
					{
						quadTree.Insert(collider);
						colliders[collidableCount] = collider;
						collidableCount++;
					}
				}
			}
		}

		for (int node = 0; node < collidableCount; ++node)
		{
			// do all of this inside the collide function
			colliders[node]->CheckCollisions(&quadTree);
		}


		free(colliders);
	}
	
	
	void Level::FixedUpdate(float delta)
	{
		QuadTree quadTree; // need to reconstruct each loop
		Collider** colliders = (Collider**) malloc(MAX_NODES * sizeof(Collider*)); // need to allocate this on the heap, too much for stack
		size_t collidableCount = 0;
		for (int node = 0; node < NodeCount; ++node)
		{
			for (int child = 0; child <= Nodes[node][0]->ChildCount; ++child)
			{
				Nodes[node][child]->FixedUpdate(delta);
				Collider* collider = dynamic_cast<Collider*>(Nodes[node][child]);
				if (collider)
				{
					if (collider->PhysicsUpdateType == PhysicsType::DISCREET)
					{
						quadTree.Insert(collider);
						colliders[collidableCount] = collider;
						collidableCount++;
					}
				}
			}
		}
		
		for (int node = 0; node < collidableCount; ++node)
		{
			// do all of this inside the collide function
			colliders[node]->CheckCollisions(&quadTree);
		}
		
		
		free(colliders);
	}
	
	
	Camera* Level::MainCamera()
	{
		if (!p_mainCamera)
			p_mainCamera = new Camera();
		
		return p_mainCamera;
	}
	
	
	Level* Level::Get()
	{
		if (!s_instance)
			s_instance = new Level();
		
		return s_instance;
	}
	
}