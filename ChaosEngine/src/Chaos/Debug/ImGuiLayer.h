#pragma once
#include "Chaos/Core/Layer.h"
#include "Vulkan/Include/vulkan/vulkan.h"
#include <ImGUI/imgui.h>

namespace Chaos
{
	class ImGuiLayer : public Layer
	{
		
		
		public:
		ImGuiLayer();
		~ImGuiLayer();
		
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override {};
		virtual void OnImGuiUpdate() override;
		
		static void Begin();
		static void End();
		private:
		void VulkanInit();
		
		private:
		VkDescriptorPool m_descriptorPool;
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkRenderPass m_renderpass;
		VkCommandPool m_commandPool;
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::vector<VkFramebuffer> m_frameBuffers;
		VkClearValue m_clear;
		
		static bool s_ImGuiInited;
	};
}