#include "chaospch.h"

#include "Texture.h"
#include "Chaos/Renderer/Renderer.h"
#include "Platform/Vulkan/VulkanTexture.h"


namespace Chaos
{
	Ref<Texture> Texture::mBlankTexture = nullptr;
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
	//if not given a file path, this will return a blank texture, should only be used once to initalise the const static BLANK_TEXTURE in the header file
	Ref<Texture> Texture::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
		case RendererAPI::API::Vulkan: return CreateRef<VulkanTexture>();
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}

	Ref<Texture> Texture::GetBlank()
	{
		if (mBlankTexture.get() == nullptr)
		{
			mBlankTexture = Create();
		}
		return mBlankTexture;
	}
}