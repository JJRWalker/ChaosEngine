/* date = April 23rd 2021 4:48 pm */

#ifndef _VULKAN_MESH_H
#define _VULKAN_MESH_H

#include "VulkanTypes.h"

#include <vector>

#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec3.h"
#include "Chaos/Renderer/Mesh.h"

namespace Chaos
{
	struct VertexInputDescription
	{
		std::vector<VkVertexInputBindingDescription> Bindings;
		std::vector<VkVertexInputAttributeDescription> Attributes;

		VkPipelineVertexInputStateCreateFlags Flags = 0;
	};

	struct Vertex
	{
		Vec3 Position;
		Vec3 Normal;
		Vec3 Colour;
		Vec2 UV;

		static VertexInputDescription GetVertexDescription();
	};

	class VulkanMesh : public Mesh
	{
		public:
		std::vector<Vertex> Vertices;
		AllocatedBuffer VertexBuffer;

		bool LoadFromOBJ(const char* filename);
	};
}

#endif //_VULKAN_MESH_H
