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
		
	private:
		float mTime = 0.0f;
		
		ImTextureID m_veiwportImageId;

		VkDescriptorPool m_descriptorPool;
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkRenderPass m_renderpass;
		VkCommandPool m_commandPool;
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::vector<VkFramebuffer> m_frameBuffers;
		VkClearValue m_clear;
	};
}