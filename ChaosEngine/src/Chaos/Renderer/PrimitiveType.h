#pragma once

#include "chaospch.h"
#include "Vertex.h"

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