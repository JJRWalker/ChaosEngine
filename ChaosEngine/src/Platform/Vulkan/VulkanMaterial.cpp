#include "chaospch.h"
#include "VulkanMaterial.h"

#include "VulkanTexture.h"
#include "VulkanRenderer.h"
#include "VulkanInitalizers.h"

namespace Chaos
{
	VulkanMaterial::VulkanMaterial(VkPipeline pipeline, VkPipelineLayout pipelineLayout, const std::string& name, VkShaderModule frag, VkShaderModule vert, VulkanRenderer* owningRenderer)
	{
		Pipeline = pipeline;
		PipelineLayout = pipelineLayout;
		Name = name;
		FragShader = frag;
		VertShader = vert;
		p_owningRenderer = owningRenderer;
	}
	
	Vec4 VulkanMaterial::GetColour() 
	{ 
		return Colour;
	}
	
	
	void VulkanMaterial::SetColour(Vec4 colour) 
	{ 
		Colour = colour;
	}
	
	
	std::string VulkanMaterial::GetName()
	{ 
		return Name;
	}
	
	
	//does nothing TODO: eventually ask the renderer for a name change
	void VulkanMaterial::SetName(std::string name) 
	{
		
	}
	
	
	Texture* VulkanMaterial::GetTexture()
	{
		return pTexture;
	}
	
	
	void VulkanMaterial::SetTexture(Texture* tex)
	{
		if (!tex)
		{
			LOGCORE_WARN("MATERIAL: SET TEXTURE: texture passed to set texture function was null, aborting texture change");
			return;
		}
		
		pTexture = (VulkanTexture*)tex;
		
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.descriptorPool = p_owningRenderer->m_descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &p_owningRenderer->m_singleTextureSetLayout;
		
		vkAllocateDescriptorSets(p_owningRenderer->m_device, &allocInfo, &TextureSet);
		
		VkDescriptorImageInfo imageBufferInfo;
		imageBufferInfo.sampler = p_owningRenderer->m_nearestNeighbourSampler;
		imageBufferInfo.imageView = pTexture->ImageView;
		imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		
		VkWriteDescriptorSet textureSet = VkInit::WriteDescriptorImage(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, TextureSet, &imageBufferInfo, 0);
		
		vkUpdateDescriptorSets(p_owningRenderer->m_device, 1, &textureSet, 0, nullptr);
	}
}

