#include "chaospch.h"

#include "EditorScene.h"
#include "Chaos/Entity/Components/Render.h"

namespace Chaos
{
	void EditorScene::StartScene()
	{
		//temp adding entities manually, should load from file
		Entity* entity = new Entity("Entity 1");
		Entity* entity2 = new Entity("Entity 2");
		entity->AddComponent<Render>();
		entity2->AddComponent<Render>();
		m_entities.push_back(entity);
		m_entities.push_back(entity2);

		for (size_t i = 0; i < 100; ++i)
		{
			Entity* otherEntity = new Entity("Stresser");
			otherEntity->AddComponent<Render>();
			m_entities.push_back(otherEntity);
		}

	}
	void EditorScene::Update()
	{
		for (auto entity : m_entities)
		{
			if (entity->GetComponent<Render>() != nullptr)
			{
				entity->GetComponent<Render>()->Update();
			}
		}
	}
}