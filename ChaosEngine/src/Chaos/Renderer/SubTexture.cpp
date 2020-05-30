#include "chaospch.h"

#include "SubTexture.h"
#include "Renderer.h"
#include "Platform/Vulkan/VulkanSubTexture.h"

namespace Chaos
{
	Ref<SubTexture> SubTexture::Create(const char* path, Vec2 coordinates, Vec2 size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
		case RendererAPI::API::Vulkan: return CreateRef<VulkanSubTexture>(path, coordinates, size);
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}
	Ref<SubTexture> SubTexture::Create(Ref<Texture> mainTexture, Vec2 coordinates, Vec2 size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
		case RendererAPI::API::Vulkan: return CreateRef<VulkanSubTexture>(mainTexture, coordinates, size);
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}
}