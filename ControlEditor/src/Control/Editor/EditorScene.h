#pragma once
#include "Chaos/Core/Scene.h"

namespace Chaos
{
	class EditorScene : public Scene
	{
	public:
		virtual void StartScene() override;
		virtual void Update() override;
	};
}