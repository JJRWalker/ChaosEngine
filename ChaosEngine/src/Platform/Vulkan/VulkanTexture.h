#pragma once
#include "Chaos/Renderer/Texture.h"
#include "Vulkan/Include/vulkan/vulkan.h"

namespace Chaos
{
	class VulkanRenderer;
	class VulkanTexture : public Texture
	{

	public:
		VulkanTexture();
		VulkanTexture(const char* filePath);

		virtual ~VulkanTexture() {};

		//Must explicitly unload textures, note if renderer doesn't exist on load or unload, it will fail to be dealocated or alocated
		virtual void Load (const char* filePath) override;
		virtual void Unload() override;

		virtual const char* GetFilePath() const override { return m_filePath; }

		virtual uint32_t GetWidth() const override { return m_width; }
		virtual uint32_t GetHeight() const override { return m_height; }
		virtual uint32_t GetSize() const override { return m_size; }
		
		VkImage& GetImage() { return m_image; }
		VkDeviceMemory& GetImageMemory() { return m_imageMemory; }
		VkImageView& GetImageView() { return m_imageView; }


	private: 
		mutable const char* m_filePath;	//not sure if having a mutable const char pointer is a good idea...
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		uint32_t m_size;
		VkImage m_image;
		VkDeviceMemory m_imageMemory;
		VkImageView m_imageView;
		//reference to the renderer we created the texture on
		VulkanRenderer& m_renderer;
		//has this texture been loaded to
		bool m_loaded = false; 
	};
}