#include "chaospch.h"
#include "Level.h"

#include "Chaos/Nodes/Node.h"
#include "Chaos/Nodes/Camera.h"

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
		for (int node = 0; node < NodeCount; ++node)
		{
			Nodes[node][0]->Update(delta);
			for (int child = 0; child < Nodes[node][0]->ChildCount; ++child)
			{
				Nodes[node][child]->Update(delta);
			}
		}
	}
	
	
	void Level::FixedUpdate(float delta)
	{
		for (int node = 0; node < NodeCount; ++node)
		{
			Nodes[node][0]->FixedUpdate(delta);
			for (int child = 0; child < Nodes[node][0]->ChildCount; ++child)
			{
				Nodes[node][child]->FixedUpdate(delta);
			}
		}
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