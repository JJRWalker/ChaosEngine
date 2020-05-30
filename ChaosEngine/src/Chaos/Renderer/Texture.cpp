#include "chaospch.h"

#include "Texture.h"
#include "Chaos/Renderer/Renderer.h"
#include "Platform/Vulkan/VulkanTexture.h"


namespace Chaos
{

		Ref<Texture> Texture::Create(const char* path)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
			case RendererAPI::API::Vulkan: return CreateRef<VulkanTexture>(path);
			}
			LOGCORE_ERROR("Render API not found");
			return nullptr;
		}

}