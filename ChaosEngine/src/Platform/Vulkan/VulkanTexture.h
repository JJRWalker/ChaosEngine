/* date = April 22nd 2021 8:45 am */

#ifndef _VULKAN_TEXTURE_H
#define _VULKAN_TEXTURE_H

#include "Chaos/Renderer/Texture.h"
#include "VulkanTypes.h"

namespace Chaos
{
	class VulkanRenderer;
	class VulkanTexture : public Texture
	{
		public:
		VulkanTexture();
		VulkanTexture(std::string filePath);
		VulkanTexture(VulkanRenderer* owningRenderer, std::string filePath);

		void Load(std::string filePath) override;
		void LoadBlank();
		void Unload() override;

		std::string GetFilePath() const override;

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		uint32_t GetSize() const override;

		private:
		void CreateImage(AllocatedBuffer& stagingBuffer, VkExtent3D imageExtent, VkFormat imageFormat);
		
		public:
		AllocatedImage Image;
		VkImageView ImageView;

		private:
		//only have getters that return value for the following, we don't want these to be changed outside of Load()
		std::string m_filePath;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		uint32_t m_size = 0;

		VulkanRenderer* p_owningRenderer = nullptr;

		bool m_loaded = false;
	};
}

#endif //_VULKAN_TEXTURE_H
