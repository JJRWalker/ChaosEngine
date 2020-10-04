#include "chaospch.h"
#include "EditorScene.h"
#include "Chaos/Entity/Systems/RenderSystem.h"
#include "Chaos/Entity/Components/CellularAutomata.h"

namespace Chaos
{
	void EditorScene::StartScene()
	{
		//temp adding entities manually, should load from file
		// Entity* entity = new Entity("Entity 1");
		// Entity* entity2 = new Entity("Entity 2");
		// entity->AddComponent<Render>();
		// entity2->AddComponent<Render>();
		// m_entities.push_back(entity);
		// m_entities.push_back(entity2);
	  
		CellularAutomata ca;
		std::vector<Node> nodes = ca.Generate(50, 50, 5, 50, 1, 3);

		for (auto node : nodes)
		  {
			Entity* nodeEnt = new Entity("Node Entity");
			nodeEnt->AddComponent<Render>();
			nodeEnt->GetComponent<Render>()->SetActive(node.Alive);
			nodeEnt->GetTransform()->Position() = Vec3(node.X - 5.0f, node.Y - 5.0f, 0.0f);
			m_entities.push_back(nodeEnt);
			//LOGCORE_TRACE("{0}", node.Neighbours.size());
		  }
		m_systems.push_back(new RenderSystem());
	}

	void EditorScene::Update()
	{
		PROFILED_FUNC();
		for (auto* sys : m_systems)
		{
		 	sys->Manage(m_entities);
		}
		// for (auto* entity : m_entities)
		//{
		//	entity->Update();
		//}
	}
	

	void EditorScene::GenerateLevel()
	{
		CellularAutomata ca; 
		std::vector<Node> nodes = ca.Generate(50, 50, 5, 28, 3, 2);

		for (int i = 0; i < m_entities.size(); ++i)
		{
			m_entities[i]->GetComponent<Render>()->SetActive(nodes[i].Alive);
		}
	}
}
