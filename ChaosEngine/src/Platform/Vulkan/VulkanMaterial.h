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
		
		
		Vec4 GetColour() override;
		void SetColour(Vec4 colour) override;
		
		std::string GetName() override;
		void SetName(std::string name) override;
		
		Texture* GetTexture() override;
		void SetTexture(Texture* tex) override;
		
		std::string Name;
		VulkanTexture* pTexture = nullptr;
		Vec4 Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
		VkDescriptorSet TextureSet = VK_NULL_HANDLE;
		VkPipeline Pipeline = VK_NULL_HANDLE;
		VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
		VkShaderModule FragShader;
		VkShaderModule VertShader;
		
		private:
		VulkanRenderer* p_owningRenderer;
	};
}

#endif //_VULKAN_MATERIAL_H
