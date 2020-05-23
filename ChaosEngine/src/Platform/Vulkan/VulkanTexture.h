#pragma once
#include "Chaos/Renderer/Texture.h"
#include "Vulkan/Include/vulkan/vulkan.h"

namespace Chaos
{
	class VulkanTexture : public Texture
	{

	public:
		VulkanTexture(const char* filePath);
		VulkanTexture(VulkanTexture& copy);

		virtual ~VulkanTexture() {};

		virtual void SetFilePath(const char* path) const override { mFilePath = path; }
		virtual const char* GetFilePath() const override { return mFilePath; }

		virtual void SetWidth(uint32_t width) { mWidth = width; }
		virtual void SetHeight(uint32_t height) { mHeight = height; }

		virtual uint32_t GetWidth() const override { return mWidth; }
		virtual uint32_t GetHeight() const override { return mHeight; }
		virtual int GetSize() const override { return mSize; }
		
		VkImage& GetImage() { return mImage; }
		VkDeviceMemory& GetImageMemory() { return mImageMemory; }
		VkImageView& GetImageView() { return mImageView; }


	private: 
		mutable const char* mFilePath;
		uint32_t mWidth = 0;
		uint32_t mHeight = 0;
		int mSize;
		int mIndex;
		VkImage mImage;
		VkDeviceMemory mImageMemory;
		VkImageView mImageView;
	};
}