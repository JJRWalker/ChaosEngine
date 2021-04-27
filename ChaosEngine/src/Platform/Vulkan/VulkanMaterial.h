/* date = April 24th 2021 9:00 am */

#ifndef _VULKAN_MATERIAL_H
#define _VULKAN_MATERIAL_H

#include "Vulkan/vulkan.h"
#include "Chaos/Renderer/Material.h"
#include <string>

namespace Chaos
{
	class VulkanMaterial : public Material
	{
	public:
		VulkanMaterial() {};
		VulkanMaterial(VkPipeline pipeline, VkPipelineLayout pipelineLayout, const std::string& name);
		VkDescriptorSet TextureSet = VK_NULL_HANDLE;
		VkPipeline Pipeline;
		VkPipelineLayout PipelineLayout;
		std::string Name;
	};
}

#endif //_VULKAN_MATERIAL_H
