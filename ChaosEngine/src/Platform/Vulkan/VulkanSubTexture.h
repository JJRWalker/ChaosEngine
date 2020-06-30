#pragma once
#include "Chaos/Renderer/SubTexture.h"
#include "VulkanTexture.h"

namespace Chaos
{
	class VulkanSubTexture : public SubTexture
	{
	public:
		//Size here corrisponds to the size of each sprite on the sprite sheet, or texture on the subtexture, e.g. if it is the size of the sprite coord [1,1] will be the entire sprite
		VulkanSubTexture(const char* filePath, Vec2 coordinates, Vec2 size);
		VulkanSubTexture(Ref<Texture> mainTexture, Vec2 coordinates, Vec2 size);
		
		virtual ~VulkanSubTexture() {};

		virtual const char* GetFilePath() const override { return mFilePath; }

		virtual const Vec2* GetTexCoords() const override { return mTexCoords; }
		virtual const Ref<Texture> GetMainTexture() const override { return mMainTexture; }

	private:
		const char* mFilePath;
		Vec2 mCoordinates;
		Vec2 mTexCoords[4];
		Ref<VulkanTexture> mMainTexture;
	};
}