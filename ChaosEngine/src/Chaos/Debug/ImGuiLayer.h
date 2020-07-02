#pragma once
#include "Chaos/Core/Layer.h"
#include "Chaos/Core/Window.h"

#include "Vulkan/Include/vulkan/vulkan.h"
#include <imgui.h>

namespace Chaos
{
	class Renderer;
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
		void VulkanDraw();
		
	private:
		float mTime = 0.0f;

		Renderer* mRenderer;
		Window* mWindow;

		std::vector<VkImage> mSwapchainImages;
		std::vector<VkImageView> mSwapchainImageViews;

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