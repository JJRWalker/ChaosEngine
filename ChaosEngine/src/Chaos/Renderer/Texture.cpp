#include "chaospch.h"

#include "Texture.h"
#include "Chaos/Renderer/Renderer.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Serialisation/FileUtils.h"

namespace Chaos
{
	Texture* Texture::Create(std::string filePath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
			case RendererAPI::API::Vulkan:
			{
				// texture should be created and then uploaded to the renderer
				// we then return a pointer to the one that exists on the renderer because we want it to have ownership
				VulkanRenderer& vkRenderer = (VulkanRenderer&)Application::Get().GetRenderer();
				return vkRenderer.CreateTexture(filePath);
			}
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}
	
	Texture* Texture::CreateBlank(std::string name)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
			case RendererAPI::API::Vulkan:
			{
				VulkanRenderer& vkRenderer = (VulkanRenderer&)Application::Get().GetRenderer();
				return vkRenderer.CreateBlankTexture(name);
			}
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}
	
	
	Texture* Texture::GetBlank()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
			case RendererAPI::API::Vulkan:
			{
				VulkanRenderer& vkRenderer = (VulkanRenderer&)Application::Get().GetRenderer();
				return vkRenderer.GetBlankTexture();
			}
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}
}