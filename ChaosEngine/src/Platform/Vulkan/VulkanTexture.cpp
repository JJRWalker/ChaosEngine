#include "chaospch.h"
#include "VulkanTexture.h"
#include <stb/stb_image.h>
#include <filesystem>

namespace Chaos
{
	VulkanTexture::VulkanTexture(const char* filePath) : mFilePath(filePath)
	{
		int texWidth, texHeight, texChannels;
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

	}
}