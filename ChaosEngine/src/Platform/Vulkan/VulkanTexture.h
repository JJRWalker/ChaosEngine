#pragma once
#include "Chaos/Renderer/Texture.h"

namespace Chaos
{
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const char* filePath);
		virtual ~VulkanTexture() {
			free(mPixelData);
			delete mPixelData;
			delete mFilePath;
		}
		virtual void SetData(void* data, uint32_t size) {};

		virtual void SetFilePath(const char* path) const override { mFilePath = path; }
		virtual const char* GetFilePath() const override { return mFilePath; }

		virtual void SetWidth(uint32_t width) { mWidth = width; }
		virtual void SetHeight(uint32_t height) { mHeight = height; }

		virtual uint32_t GetWidth() const override { return mWidth; }
		virtual uint32_t GetHeight() const override { return mHeight; }
		virtual int GetSize() const override { return mSize; }


		virtual void SetData(void* image) override { 
			size_t size = sizeof(image);
			if (image == nullptr)
			{
				LOGCORE_ERROR("VulkanTexture: failed to copy data because data was null");
				return;
			}
			free(mPixelData);
			mPixelData = malloc(size);
			memcpy(mPixelData, image, size);
			mPixelData = image; }

		inline virtual void* GetData() const override { return mPixelData; }


	private: 
		mutable const char* mFilePath;
		void* mPixelData;
		uint32_t mWidth = 0;
		uint32_t mHeight = 0;
		int mSize;
	};
}