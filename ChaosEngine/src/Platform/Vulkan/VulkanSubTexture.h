#pragma once
#include "Chaos/Renderer/SubTexture.h"
#include "VulkanTexture.h"

namespace Chaos
{
	class VulkanSubTexture : public SubTexture
	{
	public:
		//Size here corrisponds to the size of each sprite on the sprite sheet, or texture on the subtexture, e.g. if it is the size of the sprite coord [1,1] will be the entire sprite
		VulkanSubTexture(char* filePath, Vec2 coordinates, Vec2 size);
		VulkanSubTexture(Ref<Texture> mainTexture, Vec2 coordinates, Vec2 size);
		
		virtual ~VulkanSubTexture() {};

		virtual char* GetFilePath() const override { return m_filePath; }

		virtual const Vec2* GetTexCoords() const override { return m_texCoords; }
		virtual const Ref<Texture> GetMainTexture() const override { return m_mainTexture; }

	private:
		char* m_filePath;
		Vec2 m_coordinates;
		Vec2 m_texCoords[4];
		Ref<VulkanTexture> m_mainTexture;
	};
}