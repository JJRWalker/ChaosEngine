#pragma once

#include "PrimitiveType.h"

namespace Chaos
{
	class Quad : public PrimitiveType
	{
	public:
		Quad(std::vector<Vertex> verts, std::vector<uint32_t> ind, Image* texture);

		virtual std::vector<Vertex> GetVertices() override;
		virtual std::vector<uint32_t> GetIndices() override;
		virtual Image* GetImage() override;

		virtual void ClearVertices() override; 
		virtual void AddVertices(std::vector<Vertex> vertcies) override;

		virtual void ClearIndcies() override;
		virtual void AddIndcies(std::vector<uint32_t> indcies) override;

	protected:
		std::vector<Vertex> mVertcies;
		std::vector<uint32_t> mIndcies;
		Image* mTexture;
	};
}