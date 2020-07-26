#pragma once
#include "Chaos/Core/Scene.h"
#include "chaos/DataTypes/Vec2.h"
#include "chaos/entity/entity.h"
#include "Chaos/Entity/Components/Render.h"

#include <vector>

namespace Chaos
{
	class EditorScene : public Scene
	{
	public:
		virtual void StartScene() override;
		void GenerateLevel();
	};
}
