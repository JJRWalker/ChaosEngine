#include "chaospch.h"
#include "VulkanTexture.h"
#include "VulkanRenderer.h"
#include "VulkanInitalizers.h"

#include "Chaos/Core/Application.h"
#include "Chaos/Serialisation/FileUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <filesystem>

const uint32_t BLANK_PIXEL_VALUE = 0xffffffff;

namespace Chaos
{
	VulkanTexture::VulkanTexture()
	{
		p_owningRenderer = (VulkanRenderer*)&Application::Get().GetRenderer();
		LoadBlank();
	}
	
	
	VulkanTexture::VulkanTexture(VulkanRenderer* owningRenderer)
	{
		p_owningRenderer = owningRenderer;
		LoadBlank();
	}
	
	
	VulkanTexture::VulkanTexture(std::string filePath)
	{
		Name = FileUtils::GetFileName(filePath, false);
		p_owningRenderer = (VulkanRenderer*)&Application::Get().GetRenderer();
		Load(filePath);
	}
	
	
	VulkanTexture::VulkanTexture(VulkanRenderer* owningRenderer, std::string filePath)
	{
		Name = FileUtils::GetFileName(filePath, false);
		p_owningRenderer = owningRenderer;
		Load(filePath);
	}
	
	
	//tries to load the texture specified, if a texture is already loaded, it will call unload first
	void VulkanTexture::Load(std::string filePath)
	{
		if (LoadProtection)
		{
			LOGCORE_WARN("TEXTURE: trying to load into a load protected texture, are you trying to load into the blank texture?");
			return;
		}
		
		//if the file doesn't exist, either maintain the current texture or load a blank if the texture hasn't been loaded yet
		if (!std::filesystem::exists(filePath))
		{
			if (!m_loaded)
			{
				LOGCORE_WARN("TEXTURE: could not open file '{0}' Loading blank", filePath);
				LoadBlank();
				return;
			}
			LOGCORE_WARN("TEXTURE: could not open file '{0}' maintaining current texture", filePath);
			return;
		}
		if (m_loaded)
			Unload();
		//if exists then set new file path and load from there
		m_filePath = filePath;
		
		int texWidth, texHeight, texChannels;
		
		//loading in pixed data and setting width and height params based on the width and height returned from the load function
		stbi_uc* pixels = stbi_load(m_filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		if (!pixels)
		{
			LOGCORE_WARN("TEXTURE: failed to load texture {0}", filePath);
		}
		
		m_size = texWidth * texHeight * 4;
		m_width = texWidth;
		m_height = texHeight;
		
		VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
		
		AllocatedBuffer stagingBuffer = p_owningRenderer->CreateBuffer(m_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		
		// copy memory into staging buffer
		void* pixelData;
		vmaMapMemory(p_owningRenderer->m_allocator, stagingBuffer.Allocation, &pixelData);
		
		memcpy(pixelData, pixels, static_cast<size_t>(m_size));
		
		vmaUnmapMemory(p_owningRenderer->m_allocator, stagingBuffer.Allocation);
		
		VkExtent3D imageExtent;
		imageExtent.width = static_cast<uint32_t>(texWidth);
		imageExtent.height = static_cast<uint32_t>(texHeight);
		imageExtent.depth = 1;
		
		// create image with staging buffer
		CreateImage(stagingBuffer, imageExtent, imageFormat);
		
		delete pixels;
		
		m_loaded = true;
	}
	
	
	void VulkanTexture::LoadBlank()
	{
		if (m_loaded)
			Unload();
		
		Name = "blank";
		m_filePath = "";
		
		m_size = 4;
		m_width = 1;
		m_height = 1;
		
		VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
		
		AllocatedBuffer stagingBuffer = p_owningRenderer->CreateBuffer(m_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		
		void* pixelData;
		vmaMapMemory(p_owningRenderer->m_allocator, stagingBuffer.Allocation, &pixelData);
		
		memcpy(pixelData, &BLANK_PIXEL_VALUE, static_cast<size_t>(m_size));
		
		vmaUnmapMemory(p_owningRenderer->m_allocator, stagingBuffer.Allocation);
		
		VkExtent3D imageExtent;
		imageExtent.width = static_cast<uint32_t>(m_width);
		imageExtent.height = static_cast<uint32_t>(m_height);
		imageExtent.depth = 1;
		
		CreateImage(stagingBuffer, imageExtent, imageFormat);
		
		m_loaded = true;
	}
	
	
	void VulkanTexture::Unload()
	{
		m_size = 0;
		m_width = 0;
		m_height = 0;
		vmaDestroyImage(p_owningRenderer->m_allocator, Image.Image, Image.Allocation);
		vkDestroyImageView(p_owningRenderer->m_device, ImageView, nullptr);
		
		m_loaded = false;
	}
	
	
	std::string VulkanTexture::GetFilePath() const
	{
		return m_filePath;
	}
	
	
	uint32_t VulkanTexture::GetWidth() const
	{
		return m_width;
	}
	
	
	uint32_t VulkanTexture::GetHeight() const
	{
		return m_height;
	}
	
	
	uint32_t VulkanTexture::GetSize() const
	{
		return m_size;
	}
	
	
	void VulkanTexture::CreateImage(AllocatedBuffer& stagingBuffer, VkExtent3D imageExtent, VkFormat imageFormat)
	{
		VkImageCreateInfo dimageInfo = VkInit::ImageCreateInfo(imageFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);
		
		AllocatedImage newImage;
		
		VmaAllocationCreateInfo dimgAllocInfo = {};
		dimgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		
		vmaCreateImage(p_owningRenderer->m_allocator, &dimageInfo, &dimgAllocInfo, &newImage.Image, &newImage.Allocation, nullptr);
		
		p_owningRenderer->ImmediateSubmit([&](VkCommandBuffer cmd)
										  {
											  VkImageSubresourceRange range;
											  range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
											  range.baseMipLevel = 0;
											  range.levelCount = 1;
											  range.baseArrayLayer = 0;
											  range.layerCount = 1;
											  
											  VkImageMemoryBarrier imageBarrierToTransfer = {};
											  imageBarrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
											  
											  imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
											  imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
											  imageBarrierToTransfer.image = newImage.Image;
											  imageBarrierToTransfer.subresourceRange = range;
											  
											  imageBarrierToTransfer.srcAccessMask = 0;
											  imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
											  
											  vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrierToTransfer);
											  
											  VkBufferImageCopy copyRegion = {};
											  copyRegion.bufferOffset = 0;
											  copyRegion.bufferRowLength = 0;
											  copyRegion.bufferImageHeight = 0;
											  
											  copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
											  copyRegion.imageSubresource.mipLevel = 0;
											  copyRegion.imageSubresource.baseArrayLayer = 0;
											  copyRegion.imageSubresource.layerCount = 1;
											  copyRegion.imageExtent = imageExtent;
											  
											  vkCmdCopyBufferToImage(cmd, stagingBuffer.Buffer, newImage.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
											  
											  VkImageMemoryBarrier imageBarrierToReadable = imageBarrierToTransfer;
											  imageBarrierToReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
											  imageBarrierToReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
											  
											  imageBarrierToReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
											  imageBarrierToReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
											  
											  vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrierToReadable);
										  });
		
		
		vmaDestroyBuffer(p_owningRenderer->m_allocator, stagingBuffer.Buffer, stagingBuffer.Allocation);
		
		Image = newImage;
		
		VkImageViewCreateInfo imageInfo = VkInit::ImageViewCreateInfo(VK_FORMAT_R8G8B8A8_SRGB, Image.Image, VK_IMAGE_ASPECT_COLOR_BIT);
		vkCreateImageView(p_owningRenderer->m_device, &imageInfo, nullptr, &ImageView);
		
		p_owningRenderer->MainDeletionQueue.push_function([&]()
														  {
															  if (m_loaded)
																  Unload();
														  });
	}
}