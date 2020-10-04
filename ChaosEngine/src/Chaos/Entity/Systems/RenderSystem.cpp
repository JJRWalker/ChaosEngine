#include "chaospch.h"
#include "RenderSystem.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Entity/Entity.h"
#include "Chaos/Entity/Components/Render.h"
namespace Chaos
{
	void RenderSystem::Manage(std::vector<Entity*> entities)
	{
		PROFILED_FUNC();
		for (auto* entity : entities)
		{
			if (entity->HasComponent<Render>())
			{
				Render* render = entity->GetComponent<Render>();
				if (render->GetActive())
				{
					Application::Get().GetRenderer().DrawQuad(render->GetEntity()->GetTransform()->Position(), render->GetEntity()->GetTransform()->Scale(), render->GetEntity()->GetTransform()->Rotation(), render->GetTexture());
				}
			}
		}
	}
}
