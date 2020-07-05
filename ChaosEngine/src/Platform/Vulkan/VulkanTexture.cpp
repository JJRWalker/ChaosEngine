#include "chaospch.h"
#include "VulkanTexture.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Chaos/Core/Application.h"
#include <stb/stb_image.h>
#include <filesystem>

namespace Chaos
{
	//Defualt constructor, create a texture containing 1 pixel of white, doesn't call load as we don't want to trigger warnings as blank texture was probably intended
	VulkanTexture::VulkanTexture() :  m_renderer(dynamic_cast<VulkanRenderer&>(Application::Get().GetRenderer()))
	{
		VulkanRenderer& mRenderer = dynamic_cast<VulkanRenderer&>(Application::Get().GetRenderer());
		void* pixelData;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		m_filePath = "";
		//0xffffffff (1 pixel of white image data) 
		uint32_t blankPixelData = 0xffffffff;
		m_size = 4;
		m_width = 1;
		m_height = 1;
		pixelData = malloc(static_cast<size_t>(m_size));
		memcpy(pixelData, &blankPixelData, m_size);

		VkDeviceSize imageSize = m_size;

		mRenderer.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(mRenderer.m_device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixelData, static_cast<size_t>(imageSize));
		vkUnmapMemory(mRenderer.m_device, stagingBufferMemory);


		mRenderer.CreateImage(m_width, m_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_imageMemory);

		mRenderer.TransitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		mRenderer.CopyBufferToImage(stagingBuffer, m_image, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
		mRenderer.TransitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(mRenderer.m_device, stagingBuffer, nullptr);
		vkFreeMemory(mRenderer.m_device, stagingBufferMemory, nullptr);

		m_imageView = mRenderer.CreateImageView(m_image, VK_FORMAT_R8G8B8A8_SRGB);

		free(pixelData);

		m_loaded = true;
	}

	//Constructor Calls load function
	VulkanTexture::VulkanTexture(const char* filePath) : m_filePath(filePath), m_renderer(dynamic_cast<VulkanRenderer&>(Application::Get().GetRenderer()))
	{
		Load(filePath);
	}

	//tries to load the texture specified, if a texture is already loaded, it will call unload first
	void VulkanTexture::Load(const char* filePath)
	{

		if (!std::filesystem::exists(filePath))
		{
			LOGCORE_WARN("TEXTURE: could not open file '{0}' maintaining current texture", filePath);
			return;
		}
		Unload();


		//if exists then set new file path and load from there
		m_filePath = filePath;

		void* pixelData;
		int texWidth, texHeight, texChannels;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		if (!std::filesystem::exists(m_filePath))
		{
			LOGCORE_WARN("TEXTURE: could not open file '{0}' Loading blank", filePath);
			m_filePath = "";
			//0xffffffff (1 pixel of white image data) 
			uint32_t data = 0xffffffff;
			m_size = 4;
			m_width = 1;
			m_height = 1;
			pixelData = malloc(static_cast<size_t>(m_size));
			memcpy(pixelData, &data, m_size);
		}
		else
		{
			stbi_uc* pixels = stbi_load(m_filePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			m_size = texWidth * texHeight * 4;
			m_width = texWidth;
			m_height = texHeight;

			pixelData = malloc(static_cast<size_t>(m_size));

			if (!pixels) {
				LOGCORE_ERROR("TEXTURE: failed to load texture image!");
			}

			memcpy(pixelData, pixels, static_cast<size_t>(m_size));
		}

		VkDeviceSize imageSize = m_size;

		m_renderer.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(m_renderer.m_device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixelData, static_cast<size_t>(imageSize));
		vkUnmapMemory(m_renderer.m_device, stagingBufferMemory);


		m_renderer.CreateImage(m_width, m_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_imageMemory);

		m_renderer.TransitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		m_renderer.CopyBufferToImage(stagingBuffer, m_image, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
		m_renderer.TransitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(m_renderer.m_device, stagingBuffer, nullptr);
		vkFreeMemory(m_renderer.m_device, stagingBufferMemory, nullptr);

		m_imageView = m_renderer.CreateImageView(m_image, VK_FORMAT_R8G8B8A8_SRGB);

		free(pixelData);

		m_loaded = true;
	}

	void VulkanTexture::Unload()
	{
		if (m_loaded)
		{
			vkDestroyImageView(m_renderer.m_device, m_imageView, nullptr);
			vkDestroyImage(m_renderer.m_device, m_image, nullptr);
			vkFreeMemory(m_renderer.m_device, m_imageMemory, nullptr);
		}
		m_loaded = false;
	}

}