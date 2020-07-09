#include "chaospch.h"
#include "Scene.h"

namespace Chaos
{
	void Scene::StartScene()
	{
		for (auto& entity : m_entities)
		{
			entity->Start();
		}
	}
	void Scene::Update()
	{
		PROFILED_FUNC();

		for (auto& entity : m_entities)
		{
			entity->Update();
		}
	}
	void Scene::EndScene()
	{
		for (auto& entity : m_entities)
		{
			entity->Destroy();
		}
		m_entities.clear();
	}
}