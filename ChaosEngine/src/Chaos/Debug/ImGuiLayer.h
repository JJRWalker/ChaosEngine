#pragma once
#include "Chaos/Core/Layer.h"
#include "Vulkan/Include/vulkan/vulkan.h"

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
		
	private:
		float mTime = 0.0f;

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