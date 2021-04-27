#include "chaospch.h"

#include "Texture.h"
#include "Chaos/Renderer/Renderer.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Chaos/Core/Application.h"

namespace Chaos
{
	Texture* Texture::s_blankTexture = nullptr;

	Texture* Texture::Create(std::string filePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
		case RendererAPI::API::Vulkan:
			{
				// texture should be created and then uploaded to the renderer
				// we then return a pointer to the one that exists on the renderer because we want it to have ownership
				VulkanTexture tex = VulkanTexture(filePath);
				VulkanRenderer& vkRenderer = (VulkanRenderer&)Application::Get().GetRenderer();
				vkRenderer.UploadTexture(tex);
				return &vkRenderer.Textures[tex.Name];
			}
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}

	Texture* Texture::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
		case RendererAPI::API::Vulkan:
			{
				VulkanTexture tex = VulkanTexture();
				VulkanRenderer& vkRenderer = (VulkanRenderer&)Application::Get().GetRenderer();
				vkRenderer.Textures[tex.Name] = tex;
				return &vkRenderer.Textures[tex.Name];
			}
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}
	
	Texture* Texture::GetBlank()
	{
		if (!s_blankTexture)
		{
			s_blankTexture = Create();
		}
		return s_blankTexture;
	}
}