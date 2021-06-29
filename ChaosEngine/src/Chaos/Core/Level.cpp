#include "chaospch.h"
#include "Level.h"

#include "Chaos/Nodes/Node.h"
#include "Chaos/Nodes/Camera.h"
#include "Chaos/Nodes/Colliders.h"
#include "Chaos/DataTypes/QuadTree.h"
#include "Application.h"

#include <iostream>
#include <fstream>

namespace Chaos
{
	Level* Level::s_instance = nullptr;
	
	Level::Level()
	{
		memset(Nodes, 0, sizeof(Node*) * MAX_NODES * MAX_CHILD_NODES);
	}
	
	Level::~Level()
	{
		// Deleting a node shuffles the current level node array, so there will always be a [0][0] so long as the level has nodes
		while (Nodes[0][0])
		{
			delete Nodes[0][0];
		}
		
		s_instance = nullptr;
	}
	
	
	// similar to the Node::Destroy function, simply flags the level for deletion
	// this is to avoid deleting a level mid update
	void Level::Destroy()
	{
		PendingDestroy = true;
	}
	
	
	void Level::OnStart()
	{
		for (int node = 0; node < NodeCount; ++node)
		{
			for (int child = 0; child <= Nodes[node][0]->ChildCount; ++child)
			{
				Nodes[node][child]->OnStart(); 
			}
		}
	}
	
	
	void Level::OnEnd()
	{
		LOGCORE_INFO("LEVEL ENDED");
	}
	
	
	void Level::OnUpdate(float delta)
	{
		QuadTree quadTree; // need to reconstruct each loop
		Collider** colliders = (Collider**)malloc(MAX_NODES * sizeof(Collider*)); // need to allocate this on the heap, too much for stack
		size_t collidableCount = 0;
		for (int node = 0; node < NodeCount; ++node)
		{
			if (!Nodes[node][0]->IsEnabled())
				continue;
			for (int child = 0; child <= Nodes[node][0]->ChildCount; ++child)
			{
				if (Nodes[node][child]->IsEnabled())
				{
					Nodes[node][child]->OnUpdate(delta);
					if (Nodes[node][child]->DebugEnabled) 
						Nodes[node][child]->OnDebug();
					Collider* collider = dynamic_cast<Collider*>(Nodes[node][child]);
					if (collider)
					{
						quadTree.Insert(collider);
						if (collider->UpdateType == PhysicsUpdateType::PER_FRAME)
						{
							colliders[collidableCount] = collider;
							collidableCount++;
						}
					}
				}
			}
		}
		
		for (int node = 0; node < collidableCount; ++node)
		{
			// do all of this inside the collide function
			colliders[node]->CheckCollisions(&quadTree);
		}
		
		// must check to see if any have left after all have been checked.
		for (int node = 0; node < collidableCount; ++node)
		{
			colliders[node]->CheckCollisionExit();
		}
		
		free(colliders);
	}
	
	
	void Level::OnFixedUpdate(float delta)
	{
		QuadTree quadTree; // need to reconstruct each loop
		Collider** colliders = (Collider**) malloc(MAX_NODES * sizeof(Collider*)); // need to allocate this on the heap, too much for stack
		size_t collidableCount = 0;
		for (int node = 0; node < NodeCount; ++node)
		{
			if (!Nodes[node][0]->IsEnabled())
				continue;
			for (int child = 0; child <= Nodes[node][0]->ChildCount; ++child)
			{
				if (Nodes[node][child]->IsEnabled())
				{
					Nodes[node][child]->OnFixedUpdate(delta);
					Collider* collider = dynamic_cast<Collider*>(Nodes[node][child]);
					if (collider)
					{
						quadTree.Insert(collider);
						if (collider->UpdateType == PhysicsUpdateType::FIXED_STEP)
						{
							colliders[collidableCount] = collider;
							collidableCount++;
						}
					}
				}
			}
		}
		
		for (int node = 0; node < collidableCount; ++node)
		{
			// do all of this inside the collide function
			colliders[node]->CheckCollisions(&quadTree);
		}
		
		// must check to see if any have left after all have been checked.
		for (int node = 0; node < collidableCount; ++node)
		{
			colliders[node]->CheckCollisionExit();
		}
		
		free(colliders);
	}
	
	void Level::Save(const char* filePath)
	{
		std::fstream out(filePath, std::ios::out | std::ios::binary);
		
		if (!out)
			LOGCORE_ERROR("SAVE LEVEL: could not create output file!");
		
		out.write((char*)&NodeCount, sizeof(size_t));
		
		for (int node = 0; node < NodeCount; ++node)
		{
			out.write((char*)&Nodes[node][0]->ChildCount, sizeof(size_t));
			for (int child = 0; child <= Nodes[node][0]->ChildCount; ++child)
			{
				size_t nodeSize = Nodes[node][child]->GetSize();
				out.write((char*)&nodeSize, sizeof(size_t));
				out.write((char*)&(*Nodes[node][child]), Nodes[node][child]->GetSize());
			}
		}
		
		out.close();
		//out.write((char*)this, sizeof(*this));
	}
	
	void Level::Load(const char* filePath)
	{
		Application::Get().PauseFixedUpdateThread();
		std::fstream in(filePath, std::ios::in | std::ios::binary);
		
		
		if (!in)
		{
			LOGCORE_WARN("LOAD LEVEL: file path not found ({0})", filePath);
			return;
		}
		
		Level* buffer = (Level*)malloc(sizeof(Level));
		
		in.read((char*)&buffer->NodeCount, sizeof(size_t));
		
		for (int node = 0; node < buffer->NodeCount; ++node)
		{
			size_t childCount = 0;
			in.read((char*)&childCount, sizeof(size_t));
			for (int child = 0; child <= childCount; ++child)
			{
				size_t nodeSize = 0;
				in.read((char*)&nodeSize, sizeof(size_t));
				std::vector<char> data;
				data.resize(nodeSize);
				in.read(data.data(), nodeSize);
				buffer->Nodes[node][child] = (Node*)malloc(nodeSize);
				memcpy(buffer->Nodes[node][child], data.data(), nodeSize);
			}
		}
		
		//in.read((char*)buffer, sizeof(Level));
		in.close();
		memcpy(this, buffer, sizeof(*buffer));
		free(buffer);
		
		OnStart();
		
		Application::Get().ResumeFixedUpdateThread();
		
	}
	
	
	Level* Level::Get()
	{
		if (!s_instance)
			s_instance = new Level();
		
		return s_instance;
	}
	
}