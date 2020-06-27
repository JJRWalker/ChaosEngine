#pragma once
#include "chaospch.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Entity/Entity.h"

namespace Chaos
{
	class Scene
	{
	public:
		virtual void StartScene() = 0;
		virtual void Update() = 0;
	};
}