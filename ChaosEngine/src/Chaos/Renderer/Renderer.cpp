#include "chaospch.h"

#include "Renderer.h"
#include "Platform/Vulkan/VulkanRenderer.h"

namespace Chaos
{
	Renderer* Renderer::Create(Window* window)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::Vulkan:
				return new VulkanRenderer(window);

			case RendererAPI::API::None:
				LOGCORE_ERROR("RENDERER: No rendering API specified");
				return nullptr;				
		}
		LOGCORE_ERROR("RENDERER: Renderer API not supported");
		return nullptr;
	}
}