#include "chaospch.h"

#include "VulkanSubTexture.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Chaos/Core/Application.h"
#include "VulkanTexture.h"

namespace Chaos
{
	VulkanSubTexture::VulkanSubTexture(const char* filePath, Vec2 coordinates, Vec2 size) : mFilePath(filePath), mCoordinates(coordinates)
	{
		//Check if the renderer already has the texture in it's list of textures to bind, if it does it will set the main texture reference to that texture
		//if it doesn't it will create a new ref to a new texture 
		if (!Application::Get().GetRenderer().HasTexture(filePath, mMainTexture))
		{
			mMainTexture = CreateRef<VulkanTexture>(filePath);
		}

		//Setting the tex coords based on the size of the main texture and the x and y coords given
		Vec2 min = { (coordinates.X * size.X) / mMainTexture->GetWidth(), (coordinates.Y * size.Y) / mMainTexture->GetHeight() };
		Vec2 max = { ((coordinates.X + 1) * size.X) / mMainTexture->GetWidth(), ((coordinates.Y + 1) * size.Y) / mMainTexture->GetHeight() };

		mTexCoords[0] = { min.X, min.Y };
		mTexCoords[1] = { max.X, min.Y };
		mTexCoords[2] = { max.X, max.Y };
		mTexCoords[3] = { min.X, max.Y };
	}
	VulkanSubTexture::VulkanSubTexture(Ref<Texture> mainTexture, Vec2 coordinates, Vec2 size) : mFilePath(mainTexture->GetFilePath()), mCoordinates(coordinates)
	{
		mMainTexture = std::static_pointer_cast<VulkanTexture>(mainTexture);

		//Setting the tex coords based on the size of the main texture and the x and y coords given
		Vec2 min = { (coordinates.X * size.X) / mMainTexture->GetWidth(), (coordinates.Y * size.Y) / mMainTexture->GetHeight() };
		Vec2 max = { ((coordinates.X + 1) * size.X) / mMainTexture->GetWidth(), ((coordinates.Y + 1) * size.Y) / mMainTexture->GetHeight() };

		mTexCoords[0] = { min.X, min.Y };
		mTexCoords[1] = { max.X, min.Y };
		mTexCoords[2] = { max.X, max.Y };
		mTexCoords[3] = { min.X, max.Y };
	}
}