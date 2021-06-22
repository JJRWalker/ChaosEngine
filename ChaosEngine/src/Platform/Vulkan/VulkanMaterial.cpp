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
	
	
	void VulkanMaterial::CleanUp()
	{
		vkDestroyPipeline(p_owningRenderer->m_device, Pipeline, nullptr);
		vkDestroyPipelineLayout(p_owningRenderer->m_device, PipelineLayout, nullptr);
	}
	
	
	std::string VulkanMaterial::GetName()
	{ 
		return Name;
	}
	
	
	//does nothing TODO: eventually ask the renderer for a name change
	void VulkanMaterial::SetName(std::string name) 
	{
		
	}
}

