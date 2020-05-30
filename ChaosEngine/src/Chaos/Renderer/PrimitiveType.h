#pragma once

#include "chaospch.h"
#include "Vertex.h"

//DATA TYPE NO LONGER USED IN RENDERER, WILL BE REPURPOSED IN THE FUTURE FOR DRAWING ELEMENTS THAT ARE NOT QUADS (e.g. possibly some UI elements)
namespace Chaos
{
	class PrimitiveType
	{
	public:
		virtual ~PrimitiveType() {};

		virtual std::vector<Vertex> GetVertices() = 0;
		virtual std::vector<uint32_t> GetIndices() = 0;
		virtual Texture* GetTexture() = 0;
	};
}