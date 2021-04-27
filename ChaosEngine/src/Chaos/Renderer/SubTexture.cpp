#include "chaospch.h"

#include "SubTexture.h"
#include "Renderer.h"

namespace Chaos
{
	Ref<SubTexture> SubTexture::Create(std::string path, Vec2 coordinates, Vec2 size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
			case RendererAPI::API::Vulkan: return nullptr;
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}
	Ref<SubTexture> SubTexture::Create(Texture* mainTexture, Vec2 coordinates, Vec2 size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
			case RendererAPI::API::Vulkan: return nullptr;
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}
}