#pragma once

#include "chaospch.h"
#include "Image.h"
#include "Vertex.h"

namespace Chaos
{
	class PrimitiveType
	{
		PrimitiveType(); 
		virtual ~PrimitiveType() {};

		virtual std::vector<Vertex> GetVertices() = 0;
		virtual std::vector<uint32_t> GetIndices() = 0;
		virtual Image* GetImage() = 0;

		virtual void ClearVertices() = 0;
		virtual void AddVertices(std::vector<Vertex> vertcies) = 0;

		virtual void ClearIndcies() = 0;
		virtual void AddIndcies(std::vector<uint32_t> indcies) = 0;
	};
}