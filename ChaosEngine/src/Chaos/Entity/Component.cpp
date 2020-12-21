#include "Component.h"
#include <Chaos/Entity/ECSManager.h>
#include <Chaos/Entity/Entity.h>

namespace Chaos
{
	Entity* Component::GetEntity()
	{
		return ECSManager::GetEntityFromComponent(m_componentID); 
	}
	
	Vec3& Component::GetPosition()
	{
		return GetEntity()->GetPosition();
	}
}