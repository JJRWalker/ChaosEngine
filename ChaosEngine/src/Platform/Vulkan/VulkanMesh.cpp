#include "chaospch.h"
#include "VulkanMesh.h"

namespace Chaos
{
	VertexInputDescription VulkanVertex::GetVertexDescription()
	{
		VertexInputDescription description;
		
		VkVertexInputBindingDescription mainBinding = {};
		mainBinding.binding = 0;
		mainBinding.stride = sizeof(Vertex);
		mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		description.Bindings.push_back(mainBinding);
		
		VkVertexInputAttributeDescription positionAttribute = {};
		positionAttribute.binding = 0;
		positionAttribute.location = 0;
		positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionAttribute.offset = offsetof(Vertex, Position);
		
		VkVertexInputAttributeDescription normalAttribute = {};
		normalAttribute.binding = 0;
		normalAttribute.location = 1;
		normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		normalAttribute.offset = offsetof(Vertex, Normal);
		
		VkVertexInputAttributeDescription colourAttribute = {};
		colourAttribute.binding = 0;
		colourAttribute.location = 2;
		colourAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		colourAttribute.offset = offsetof(Vertex, Colour);
		
		VkVertexInputAttributeDescription UVAttribute = {};
		UVAttribute.binding = 0;
		UVAttribute.location = 3;
		UVAttribute.format = VK_FORMAT_R32G32_SFLOAT;
		UVAttribute.offset = offsetof(Vertex, UV);
		
		description.Attributes.push_back(positionAttribute);
		description.Attributes.push_back(normalAttribute);
		description.Attributes.push_back(colourAttribute);
		description.Attributes.push_back(UVAttribute);
		
		return description;
	}
	
}