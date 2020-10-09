#include "chaospch.h"
#include "Entity.h"

namespace Chaos
{
	void Entity::Start() 
	{
		for (auto* c : GetAllComponents())
		{
			c->Start();
		}
	}
	void Entity::Update() 
	{
		for (auto* c : GetAllComponents())
		{
			if (c->GetActive())
				c->Update();
		}
	}
	void Entity::Destroy()
	{
		for (auto* c : GetAllComponents())
		{
			c->Destroy();
		}
	}
	
	void Entity::Debug()
	{
		for (auto* c : GetAllComponents())
		{
			c->Debug();
		}
	}
}