#include "chaospch.h"
#include "VulkanMesh.h"

#include <TinyObjLoader/tiny_obj_loader.h>
#include "TinyObjLoader/tiny_obj_loader.cc"
#include <iostream>

namespace Chaos
{
	VertexInputDescription Vertex::GetVertexDescription()
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


	bool VulkanMesh::LoadFromOBJ(const char* filename)
	{
		tinyobj::attrib_t attrib;

		std::vector<tinyobj::shape_t> shapes;

		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;

		tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, nullptr);

		if (!warn.empty())
			LOGCORE_WARN("OBJ LOADER: {0}", warn);

		if (!err.empty())
			LOGCORE_ERROR("OBJ LOADER: {0}", err);

		for (size_t s = 0; s < shapes.size(); ++s)
		{
			size_t indexOffset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
			{
				int fv = 3;

				for (size_t v = 0; v < fv; ++v)
				{
					tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];

					tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
					tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
					tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

					tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
					tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
					tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

					tinyobj::real_t cx = attrib.colors[3 * idx.vertex_index + 0];
					tinyobj::real_t cy = attrib.colors[3 * idx.vertex_index + 1];
					tinyobj::real_t cz = attrib.colors[3 * idx.vertex_index + 2];

					tinyobj::real_t ux = attrib.texcoords[2 * idx.texcoord_index + 0];
					tinyobj::real_t uy = attrib.texcoords[2 * idx.texcoord_index + 1];

					Vertex newVert;
					newVert.Position.X = vx;
					newVert.Position.Y = vy;
					newVert.Position.Z = vz;

					newVert.Normal.X = nx;
					newVert.Normal.Y = ny;
					newVert.Normal.Z = nz;

					newVert.Colour.X  = cx;
					newVert.Colour.Y  = cy;
					newVert.Colour.Z  = cz;

					newVert.UV.X = ux;
					newVert.UV.Y = 1-uy;

					Vertices.push_back(newVert);
				}
				indexOffset += fv;
			}
		}

		return true;
	}
}