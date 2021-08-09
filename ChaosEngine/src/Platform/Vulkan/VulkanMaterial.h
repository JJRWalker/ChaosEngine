/* date = April 24th 2021 9:00 am */

#ifndef _VULKAN_MATERIAL_H
#define _VULKAN_MATERIAL_H

#include "Vulkan/vulkan.h"
#include "Chaos/Renderer/Material.h"
#include <string>

namespace Chaos
{
	class VulkanTexture;
	class VulkanRenderer;
	class VulkanMaterial : public Material
	{
		public:
		VulkanMaterial() {};
		VulkanMaterial(VkPipeline pipeline, VkPipelineLayout pipelineLayout, const std::string& name, VkShaderModule frag, VkShaderModule vert, VulkanRenderer* owningRenderer);
		
		void CleanUp();
		
		
		std::string GetName() override;
		void SetName(std::string name) override;
		
		std::string Name;
		Vec4 Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
		VkPipeline Pipeline = VK_NULL_HANDLE;
		VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
		VkShaderModule FragShader;
		VkShaderModule VertShader;
		
		private:
		VulkanRenderer* p_owningRenderer;
	};
}

#endif //_VULKAN_MATERIAL_H
