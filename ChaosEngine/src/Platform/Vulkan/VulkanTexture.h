#pragma once
#include "Chaos/Renderer/Texture.h"

#include "Chaos/Renderer/Renderer.h"

namespace Chaos
{
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const char* filePath) : mFilePath(filePath) {};
		virtual ~VulkanTexture() {
			delete mImage;
			delete mFilePath;
		}
		virtual void SetData(void* data, uint32_t size) {};

		virtual void SetFilePath(const char* path) const override { mFilePath = path; }
		virtual const char* GetFilePath() const override { return mFilePath; }

		virtual void SetWidth(uint32_t width) { mWidth = width; }
		virtual void SetHeight(uint32_t height) { mHeight = height; }

		virtual uint32_t GetWidth() const override { return mWidth; }
		virtual uint32_t GetHeight() const override { return mHeight; }


		void SetData(VkImage* image) { mImage = image; }

		virtual VkImage* GetImage() { return mImage; }


	private: 
		mutable const char* mFilePath;
		VkImage* mImage;
		uint32_t mWidth = 0;
		uint32_t mHeight = 0;

	};
}