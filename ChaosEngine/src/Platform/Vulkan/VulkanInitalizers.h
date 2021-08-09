/* date = April 22nd 2021 8:40 am */

#ifndef _VULKAN_INITALIZERS_H
#define _VULKAN_INITALIZERS_H

#include "VulkanTypes.h"

namespace VkInit
{
	VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);

	VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass renderpass, VkExtent2D extent);

	VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags = 0);

	VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);

	VkSubmitInfo SubmitInfo(VkCommandBuffer* cmd);

	VkPresentInfoKHR PresentInfo();

	VkRenderPassBeginInfo RenderpassBeginInfo(VkRenderPass renderpass, VkExtent2D windowExtent, VkFramebuffer framebuffer);

	VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shaderModule);

	VkPipelineVertexInputStateCreateInfo VertexInputCreateInfo();

	VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo(VkPrimitiveTopology topology);

	VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo(VkPolygonMode polygonMode);

	VkPipelineMultisampleStateCreateInfo MultisamlingStateCreateInfo();

	VkPipelineColorBlendAttachmentState ColourBlendAttachmentState();

	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo();

	VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);

	VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);

	VkPipelineDepthStencilStateCreateInfo DepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);

	VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding);

	VkWriteDescriptorSet WriteDescriptorBuffer(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo, uint32_t binding);

	VkSamplerCreateInfo SamplerCreateInfo(VkFilter filters, VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

	VkWriteDescriptorSet WriteDescriptorImage(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, uint32_t binding);
}

#endif //_VULKAN_INITALIZERS_H
