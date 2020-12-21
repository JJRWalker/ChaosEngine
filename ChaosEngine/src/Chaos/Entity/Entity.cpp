#include "chaospch.h"
#include "Entity.h"

namespace Chaos
{
	//called on scene start, found in Scene.h
	void Entity::Start() 
	{
		for (auto* c : GetAllComponents())
		{
			c->Start();
		}
	}
	
	//Updates every frame / application loop found in Application.h
	void Entity::Update() 
	{
		for (auto* c : GetAllComponents())
		{
			if (c->GetActive())
				c->Update();
		}
	}
	
	//called every fixed timestep determined by the Time class in Time.h
	void Entity::FixedUpdate()
	{
		for (auto* c : GetAllComponents())
		{
			if(c->GetActive())
				c->FixedUpdate();
		}
	}
	
	//currently just propogates to all it's components, doesn't actually have a function to handle it itself, however this could be overriden and the base could be called, then some extra functionality could happen
	void Entity::ColliderHit(Collider* self, Collider* other)
	{
		for (auto* c : GetAllComponents())
		{
			c->ColliderHit(self, other);
		}
	}
	
	void Entity::TriggerHit(Collider* self, Collider* other)
	{
		for (auto* c : GetAllComponents())
		{
			c->TriggerHit(self, other);
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