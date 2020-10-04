#pragma once
#include "Chaos/Entity/Entity.h"

namespace Chaos
{
	class Player : public Entity
	{
		public:
		Player() : Entity("Player")
		{
			
		}
		
		void Start() override;
	};
}
