#pragma once

#include "Chaos/Renderer/PrimitiveType.h"
#include "Chaos/Renderer/Texture.h"

namespace Chaos
{
	class Quad : public PrimitiveType
	{
	public:
		Quad(std::vector<Vertex> verts, std::vector<uint32_t> ind, Texture* texture)
			: mVertcies(verts), mIndcies(ind), mTexture(texture) {};

		virtual std::vector<Vertex> GetVertices() override { return mVertcies; }
		virtual std::vector<uint32_t> GetIndices() override { return mIndcies; }
		virtual Texture* GetTexture() { return mTexture; }

	protected:
		std::vector<Vertex> mVertcies;
		std::vector<uint32_t> mIndcies;
		Texture* mTexture;
	};
}