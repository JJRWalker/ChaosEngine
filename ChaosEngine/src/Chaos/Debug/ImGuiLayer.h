#pragma once
#include "Chaos/Core/Layer.h"

#include "Vulkan/Include/vulkan/vulkan.h"

namespace Chaos
{


	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() ;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		void VulkanInit();
		
	private:
		float mTime = 0.0f;

		VkDevice device;

		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorSet descriptorSet;
		VkRenderPass renderpass;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkFramebuffer> frameBuffers;
		VkClearValue clear;

		/*
		VkSampler sampler;
		VkBuffer vertexBuffer;
		VkBuffer indexBuffer;
		int32_t vertexCount = 0;
		int32_t indexCount = 0;
		VkDeviceMemory fontMemory = VK_NULL_HANDLE;
		VkImage fontImage = VK_NULL_HANDLE;
		VkImageView fontView = VK_NULL_HANDLE;
		VkPipelineCache pipelineCache;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		*/
	};
}