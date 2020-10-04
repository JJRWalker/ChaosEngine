#pragma once
#include "Chaos/Core/Scene.h"
#include "chaos/DataTypes/Vec2.h"
#include "chaos/entity/entity.h"
#include "Chaos/Entity/Components/Render.h"

#include <vector>

namespace Chaos
{
	class System;
	class EditorScene : public Scene
	{
	public:
		virtual void StartScene() override;
		virtual void Update() override;
		void GenerateLevel();
	private:
		std::vector<System*> m_systems;
	};
}
