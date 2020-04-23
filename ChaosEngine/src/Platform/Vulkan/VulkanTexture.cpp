#include "chaospch.h"
#include "VulkanTexture.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Chaos/Core/Application.h"
#include <stb/stb_image.h>
#include <filesystem>

namespace Chaos
{
	VulkanTexture::VulkanTexture(const char* filePath, float tilingFactor) : mFilePath(filePath), mTilingFactor(tilingFactor)
	{
		int texWidth, texHeight, texChannels;
		VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(Application::Get().GetRenderer());
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		if (!std::filesystem::exists(mFilePath))
		{
			LOGCORE_WARN("TEXTURE: could not open file '{0}' Loading blank", filePath);
			mFilePath = "../Game/textures/blank.png";
		}

		stbi_uc* pixels = stbi_load(mFilePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		mSize = texWidth * texHeight * 4;
		mWidth = texWidth;
		mHeight = texHeight;

		mPixelData = malloc(static_cast<size_t>(mSize));

		if (!pixels) {
			LOGCORE_ERROR("TEXTURE: failed to load texture image!");
		}

		memcpy(mPixelData, pixels, static_cast<size_t>(mSize));

		VkDeviceSize imageSize = texWidth * texHeight * 4;

		renderer.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(renderer.mDevice, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, mPixelData, static_cast<size_t>(imageSize));
		vkUnmapMemory(renderer.mDevice, stagingBufferMemory);


		renderer.CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mImage, mImageMemory);

		renderer.TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		renderer.CopyBufferToImage(stagingBuffer, mImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		renderer.TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(renderer.mDevice, stagingBuffer, nullptr);
		vkFreeMemory(renderer.mDevice, stagingBufferMemory, nullptr);

		mImageView = renderer.CreateImageView(mImage, VK_FORMAT_R8G8B8A8_SRGB);

		delete mPixelData;
	}
	VulkanTexture::VulkanTexture(VulkanTexture& copy)
	{
		mFilePath = copy.GetFilePath();
		mTilingFactor = copy.GetTilingFactor();
		mWidth = copy.GetWidth();
		mHeight = copy.GetHeight();
		mSize = copy.GetSize();
	}
}