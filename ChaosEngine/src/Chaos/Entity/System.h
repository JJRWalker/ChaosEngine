#pragma once
#include <vector>
#include "Chaos/Entity/Entity.h"
namespace Chaos
{
	class System
	{
	public:
		virtual void Manage(std::vector<Entity*> entities) = 0;
	};
}
