#pragma once
#include "Chaos/Core/Layer.h"
#include "Vulkan/Include/vulkan/vulkan.h"
#include <imgui.h>

namespace Chaos
{
	class ImGuiLayer : public Layer
	{


	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override { mTime = deltaTime; }
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		void VulkanInit();
		void CreateSwapChain();
		void CreateImageViews();
		
	private:
		float mTime = 0.0f;

		uint32_t id = 0;

		VkSwapchainKHR mSwapchain;
		std::vector<VkImage> mSwapchainImages;
		std::vector<VkImageView> mSwapchainImageViews;
		
		ImTextureID mVeiwportImageId;

		VkDescriptorPool mDescriptorPool;
		VkDescriptorSetLayout mDescriptorSetLayout;
		VkDescriptorSet descriptorSet;
		VkRenderPass renderpass;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> mCommandBuffers;
		std::vector<VkFramebuffer> frameBuffers;
		VkClearValue clear;
	};
}