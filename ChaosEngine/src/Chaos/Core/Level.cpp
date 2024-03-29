#include "chaospch.h"
#include "Level.h"

#include "Application.h"
#include "Chaos/Nodes/Node.h"
#include "Chaos/Nodes/Camera.h"
#include "Chaos/Nodes/Sprite.h"
#include "Chaos/Nodes/MeshRenderer.h"
#include "Chaos/Nodes/Colliders.h"
#include "Chaos/DataTypes/QuadTree.h"
#include "Chaos/Serialisation/Binary.h"
#include "Chaos/Core/Types.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>

namespace Chaos
{
	Level* Level::s_instance = nullptr;
	
	Level::Level()
	{
		memset(Nodes, 0, sizeof(Node*) * MAX_NODES);
	}
	
	Level::~Level()
	{
		while(Nodes[0])
		{
			delete Nodes[0];
		}
		
		s_instance = nullptr;
	}
	
	
	void Level::OnStart()
	{
		LOGCORE_INFO("LEVEL STARTED");
		for (int node = 0; node < NodeCount; ++node)
		{
			Nodes[node]->OnStart();
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
			if (!Nodes[node]->IsEnabled())
				continue;
			
			if (Nodes[node]->DebugEnabled) 
				Nodes[node]->OnDebug();
			
			Nodes[node]->OnUpdate(delta);
			
			if (Collider* collider = dynamic_cast<Collider*>(Nodes[node]))
			{
				quadTree.Insert(collider);
				if (collider->UpdateType == EPhysicsUpdateType::PER_FRAME)
				{
					colliders[collidableCount] = collider;
					collidableCount++;
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
			if (!Nodes[node]->IsEnabled())
				continue;
			
			Nodes[node]->OnFixedUpdate(delta);
			
			if (Collider* collider = dynamic_cast<Collider*>(Nodes[node]))
			{
				quadTree.Insert(collider);
				if (collider->UpdateType == EPhysicsUpdateType::FIXED_STEP)
				{
					colliders[collidableCount] = collider;
					collidableCount++;
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
	
	
	// Update only renderables, nothing else
	void Level::OnEditorUpdate(float delta)
	{
		for (int node = 0; node < NodeCount; ++node)
		{
			if (!Nodes[node]->IsEnabled())
				continue;
			
			if (!(Nodes[node]->Type == NodeType::CAMERA 
				  ||Nodes[node]->Type == NodeType::SPRITE
				  ||Nodes[node]->Type == NodeType::SUB_SPRITE
				  ||Nodes[node]->Type == NodeType::UI_SPRITE
				  ||Nodes[node]->Type == NodeType::MESH_RENDERER
				  ||Nodes[node]->Type == NodeType::ANIMATOR
				  ||Nodes[node]->Type == NodeType::POINT_LIGHT))
				continue;
			
			if (Nodes[node]->DebugEnabled) 
				Nodes[node]->OnDebug();
			
			Nodes[node]->OnUpdate(delta);
		}
	}
	
	
	void Level::Clear()
	{
		Application::Get().AddPostUpdateCallback([&](){
													 delete s_instance;
													 s_instance = nullptr;
												 });
	}
	
	
	void Level::Save(const char* filePath)
	{
		s_instance = Get();
		size_t finalBinarySize = 0;
		
		std::fstream out(filePath, std::ios::out | std::ios::binary);
		
		if (!out)
			LOGCORE_ERROR("SAVE LEVEL: could not create output file!");
		
		
		out.write((char*)&s_instance->GraphicalData, sizeof(GPULevelData));
		
		size_t rootCount = 0;
		
		// have to get the total number of root nodes first, then save all them
		for (int node = 0; node < s_instance->NodeCount; ++node)
		{
			// if they don't have a parent, it means they are a root node.
			// ignore child nodes as each node will save all it's children
			if (!s_instance->Nodes[node]->Parent)
			{
				++rootCount;
			}
		}
		
		out.write((char*)&rootCount, sizeof(size_t));
		
		// actually save them
		for (int node = 0; node < s_instance->NodeCount; ++node)
		{
			if (!s_instance->Nodes[node]->Parent)
			{
				Binary nodeBinary = s_instance->Nodes[node]->SaveToBinary();
				size_t nodeSize = nodeBinary.Capacity();
				
				out.write((char*)&nodeSize, sizeof(size_t));
				out.write(nodeBinary.Data, nodeBinary.Capacity());
			}
		}
		
		out.close();
	}
	
	
	void Level::Load(const char* filepath)
	{
		s_instance->m_filepathToLoad = std::string(filepath);
		Application::Get().AddPostUpdateCallback([&](){
													 Level::InternalLoad();
												 });
	}
	
	
	void Level::InternalLoad()
	{
		std::fstream in(s_instance->m_filepathToLoad.c_str(), std::ios::in | std::ios::binary);
		
		if (!in)
		{
			LOGCORE_WARN("LOAD LEVEL: file path not found ({0})", s_instance->m_filepathToLoad);
			return;
		}
		
		in.seekg(0, in.end);
		
		size_t fileSize = in.tellg();
		char* rawData = (char*)malloc(fileSize);
		in.seekg(0, in.beg);
		in.read(&rawData[0], fileSize);
		
		in.close();
		
		size_t location = 0;
		
		delete s_instance;
		
		Level* buffer = new Level();
		
		s_instance = buffer;
		
		memcpy((void*)&buffer->GraphicalData, (void*)&rawData[location], sizeof(GPULevelData));
		
		location += sizeof(GPULevelData);
		
		size_t nodeCount;
		
		memcpy((void*)&nodeCount, (void*)&rawData[location], sizeof(size_t));
		location += sizeof(size_t);
		
		for (int node = 0; node < nodeCount; ++node)
		{
			size_t nodeSize;
			memcpy((void*)&nodeSize, (void*)&rawData[location], sizeof(size_t));
			location += sizeof(size_t);
			
			char* nodeData = (char*)malloc(nodeSize);
			memcpy((void*)nodeData, (void*)&rawData[location], nodeSize);
			
			uint32_t nodeType;
			// skip over the version number to get the type
			memcpy((void*)&nodeType, (void*)&nodeData[sizeof(uint32_t)], sizeof(uint32_t));
			
			Node* spawnedNode = Node::Create(nodeType);
			spawnedNode->LoadFromBinary(nodeData);
			
			location += nodeSize;
			
			free(nodeData);
		}
		
		free(rawData);
		
#ifndef CHAOS_EDITOR
		buffer->OnStart();
#endif
	}
	
	
	Level* Level::Get()
	{
		if (!s_instance)
			s_instance = new Level();
		
		return s_instance;
	}
	
}