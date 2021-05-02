#include "chaospch.h"
#include "Material.h"
#include "Renderer.h"

#include "Chaos/Core/Application.h"
#include "Platform/Vulkan/VulkanRenderer.h"
namespace Chaos
{
	Material* Material::Create(std::string name, Texture* texture, std::string fragShaderPath, std::string vertShaderPath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
			case RendererAPI::API::Vulkan:
			{
				VulkanRenderer& vkRenderer = (VulkanRenderer&)Application::Get().GetRenderer();
				return vkRenderer.CreateMaterial(name, texture, fragShaderPath, vertShaderPath);
			}
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}
	
	
	Material* Material::Get(std::string name)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: LOGCORE_ERROR("Render API not supported!"); return nullptr;
			case RendererAPI::API::Vulkan:
			{
				VulkanRenderer& vkRenderer = (VulkanRenderer&)Application::Get().GetRenderer();
				return &vkRenderer.Materials[name];
			}
		}
		LOGCORE_ERROR("Render API not found");
		return nullptr;
	}
}