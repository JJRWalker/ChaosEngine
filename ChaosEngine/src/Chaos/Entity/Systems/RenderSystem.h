#pragma once
#include "Chaos/Entity/System.h"

namespace Chaos
{
	class RenderSystem : public System
	{
	public:
		RenderSystem() {};
		virtual void Manage(std::vector<Entity*> entities) override;
	};  
}
