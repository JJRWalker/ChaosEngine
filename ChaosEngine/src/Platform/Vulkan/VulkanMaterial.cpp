#include "chaospch.h"
#include "VulkanMaterial.h"

namespace Chaos
{
	VulkanMaterial::VulkanMaterial(VkPipeline pipeline, VkPipelineLayout pipelineLayout, const std::string& name)
	{
		Pipeline = pipeline;
		PipelineLayout = pipelineLayout;
		Name = name;
	}
}

