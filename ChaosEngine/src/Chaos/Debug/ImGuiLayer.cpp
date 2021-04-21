#include "chaospch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include "ImGUI/examples/imgui_impl_vulkan.h"
#include "ImGUI/examples/imgui_impl_glfw.h"
#include "Chaos/Core/Application.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanTexture.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif // !GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <GLM/glm/glm.hpp>

#define VK_CHECK_RESULT(x) {if (x != VK_SUCCESS) { LOGCORE_ERROR("IMGUI: Failed to create vulkan object");}}

namespace Chaos
{
	bool ImGuiLayer::s_ImGuiInited = false;
	
	static void check_vk_result(VkResult err)
	{
		if (err == 0) return;
		LOGCORE_ERROR("IMGUI: vulkan error: {0}", err);
	}
	struct PushConstBlock {
		glm::vec2 scale;
		glm::vec2 translate;
	} pushConstBlock;
	
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}
	
	ImGuiLayer::~ImGuiLayer()
	{
	}
	
	void ImGuiLayer::OnAttach()
	{
		//We only want to init imgui on attach if it's not already been inited, therefore we use a static bool here to ensure that if on attach isn't overriden it doesn't cause a crash when we try to init imgui again
		if (!s_ImGuiInited)
		{
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
			//io.ConfigViewportsNoAutoMerge = true;
			//io.ConfigViewportsNoTaskBarIcon = true;
			
			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsClassic();
			
			// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
			ImGuiStyle& style = ImGui::GetStyle();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}
			
			//Setting colours
			//Honestly these are pretty terrible...
			ImVec4* colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_Text]                   = ImVec4(0.80f, 0.70f, 0.70f, 1.00f);
			colors[ImGuiCol_WindowBg]               = ImVec4(0.03f, 0.03f, 0.03f, 0.34f);
			colors[ImGuiCol_PopupBg]                = ImVec4(0.03f, 0.00f, 0.07f, 0.99f);
			colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
			colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.02f, 0.19f, 0.72f);
			colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.15f, 0.04f, 0.41f, 0.40f);
			colors[ImGuiCol_FrameBgActive]          = ImVec4(0.08f, 0.02f, 0.25f, 0.67f);
			colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.00f, 0.23f, 0.39f);
			colors[ImGuiCol_TitleBgActive]          = ImVec4(0.04f, 0.00f, 0.23f, 0.39f);
			colors[ImGuiCol_MenuBarBg]              = ImVec4(0.09f, 0.00f, 0.17f, 0.39f);
			colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.01f, 0.00f, 0.01f, 0.51f);
			colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.07f, 0.04f, 0.06f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.43f, 0.22f, 0.64f, 1.00f);
			colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
			colors[ImGuiCol_SliderGrab]             = ImVec4(0.33f, 0.20f, 0.57f, 1.00f);
			colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.30f, 0.30f, 0.52f, 1.00f);
			colors[ImGuiCol_Button]                 = ImVec4(0.12f, 0.00f, 0.20f, 0.39f);
			colors[ImGuiCol_ButtonHovered]          = ImVec4(0.53f, 0.28f, 0.71f, 0.39f);
			colors[ImGuiCol_ButtonActive]           = ImVec4(0.07f, 0.00f, 0.64f, 0.39f);
			colors[ImGuiCol_Header]                 = ImVec4(0.02f, 0.02f, 0.03f, 0.70f);
			colors[ImGuiCol_HeaderHovered]          = ImVec4(0.17f, 0.07f, 0.28f, 0.60f);
			colors[ImGuiCol_HeaderActive]           = ImVec4(0.17f, 0.04f, 0.19f, 1.00f);
			colors[ImGuiCol_Separator]              = ImVec4(0.16f, 0.06f, 0.19f, 0.49f);
			colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.36f, 0.20f, 0.38f, 0.78f);
			colors[ImGuiCol_SeparatorActive]        = ImVec4(0.19f, 0.14f, 0.32f, 1.00f);
			colors[ImGuiCol_ResizeGrip]             = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
			colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.36f, 0.32f, 0.77f, 0.67f);
			colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.29f, 0.23f, 0.96f, 0.95f);
			colors[ImGuiCol_Tab]                    = ImVec4(0.12f, 0.00f, 0.20f, 0.86f);
			colors[ImGuiCol_TabHovered]             = ImVec4(0.73f, 0.18f, 0.62f, 0.80f);
			colors[ImGuiCol_TabActive]              = ImVec4(0.18f, 0.03f, 0.27f, 1.00f);
			colors[ImGuiCol_TabUnfocused]           = ImVec4(0.15f, 0.08f, 0.24f, 0.97f);
			colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.10f, 0.02f, 0.28f, 1.00f);
			colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.48f, 0.16f, 0.75f, 0.35f);
			
			
			
			style.WindowBorderSize = 0.1f;
			style.WindowRounding = 5.0f;
			
			Application& app = Application::Get();
			GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
			VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(app.GetRenderer());
			
			VulkanInit();
			
			// Setup Platform/Renderer bindings		
			ImGui_ImplGlfw_InitForVulkan(window, true);
			
			// Setup Platform/Renderer bindings
			
			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = renderer.m_instance;
			init_info.PhysicalDevice = renderer.m_physicalDevice;
			init_info.Device = renderer.m_device;
			init_info.QueueFamily = renderer.FindQueueFamilies(renderer.m_physicalDevice).graphicsFamily.value();
			init_info.Queue = renderer.m_presentQueue;
			init_info.PipelineCache = VK_NULL_HANDLE;
			init_info.DescriptorPool = m_descriptorPool;
			init_info.Allocator = nullptr;
			init_info.MinImageCount = 2;
			init_info.ImageCount = 2;
			init_info.CheckVkResultFn = check_vk_result;
			ImGui_ImplVulkan_Init(&init_info, m_renderpass);
			
			VkCommandBuffer commandBuffer = renderer.BeginSingleTimeCommands();
			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
			renderer.EndSingleTimeCommands(commandBuffer);
			
			m_clear = { 0.0f, 0.0f, 0.0f, 0.0f };
			
			s_ImGuiInited = true;
		}
	}
	
	void ImGuiLayer::OnDetach()
	{
		Application& app = Application::Get();
		VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(app.GetRenderer());
		vkDestroyRenderPass(renderer.m_device, m_renderpass, nullptr);
		
		vkFreeCommandBuffers(renderer.m_device, m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
		vkDestroyCommandPool(renderer.m_device, m_commandPool, nullptr);
		
		// Resources to destroy when the program ends
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		vkDestroyDescriptorPool(renderer.m_device, m_descriptorPool, nullptr);
	}
	
	void ImGuiLayer::Begin()
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	
	void ImGuiLayer::OnImGuiUpdate()
	{
		
	}
	
	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight());
		VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(Application::Get().GetRenderer());
		//Rendering			   
		ImGui::Render();
		
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		
		uint32_t mImageIndex = renderer.m_imageIndex;
		
		//Create command bufferr
		m_commandBuffers.resize(renderer.m_swapchainImageViews.size());
		
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandPool = m_commandPool;
		commandBufferAllocateInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();
		vkAllocateCommandBuffers(renderer.m_device, &commandBufferAllocateInfo, m_commandBuffers.data());
		
		m_frameBuffers.resize(renderer.m_swapchainImageViews.size());
		
		//Create frame buffers
		for (size_t i = 0; i < renderer.m_swapchainImageViews.size(); ++i)
		{
			VkImageView attachments[] = {
				renderer.m_swapchainImageViews[i]
			};
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_renderpass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = renderer.m_swapchainExtent.width;
			framebufferInfo.height = renderer.m_swapchainExtent.height;
			framebufferInfo.layers = 1;
			
			if (vkCreateFramebuffer(renderer.m_device, &framebufferInfo, nullptr, &m_frameBuffers[i]) != VK_SUCCESS)
			{
				LOGCORE_ERROR("IMGUI: failed to create framebuffer!");
			}
		}
		
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		for (size_t i = 0; i < m_commandBuffers.size(); ++i)
		{
			VkRenderPassBeginInfo renderpassinfo = {};
			renderpassinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderpassinfo.renderPass = m_renderpass;
			renderpassinfo.framebuffer = m_frameBuffers[i];
			renderpassinfo.renderArea.extent.width = renderer.m_swapchainExtent.width;
			renderpassinfo.renderArea.extent.height = renderer.m_swapchainExtent.height;
			renderpassinfo.clearValueCount = 1;
			renderpassinfo.pClearValues = &m_clear;
			
			vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo);
			
			//vkCmdBindPipeline(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer.mGraphicsPipeline);
			vkCmdBeginRenderPass(m_commandBuffers[i], &renderpassinfo, VK_SUBPASS_CONTENTS_INLINE);
			
			
			
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_commandBuffers[i]);
			
			vkCmdEndRenderPass(m_commandBuffers[i]);
			vkEndCommandBuffer(m_commandBuffers[i]);
		}
		renderer.SetImGuiCommandBuffer(m_commandBuffers);	
		renderer.SetImGuiCommandPool(&m_commandPool);
		renderer.SetImGuiFramebuffer(&m_frameBuffers);
		renderer.m_renderingGUI = true;
	}
	
	void ImGuiLayer::VulkanInit()
	{
		ImGuiIO& io = ImGui::GetIO();
		
		Application& app = Application::Get();
		VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(app.GetRenderer());
		
		VkDescriptorPoolSize poolSizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
		poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes);
		poolInfo.pPoolSizes = poolSizes;
		
		if (vkCreateDescriptorPool(renderer.m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to allocate descriptor pool!");
		}
		
		
		//Create command pool
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = renderer.FindQueueFamilies(renderer.m_physicalDevice).graphicsFamily.value();
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		
		if (vkCreateCommandPool(renderer.m_device, &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
			LOGCORE_ERROR("IMGUI: Could not create graphics command pool");
		}
		
		VkAttachmentDescription attachment = {};
		attachment.format = renderer.m_swapchainImageFormat;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		
		VkAttachmentReference colorAttachment = {};
		colorAttachment.attachment = 0;
		colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachment;
		
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		
		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.dependencyCount = 1;
		info.pDependencies = &dependency;
		
		if (vkCreateRenderPass(renderer.m_device, &info, nullptr, &m_renderpass) != VK_SUCCESS) {
			LOGCORE_ERROR("IMGUI: Could not create ImGui's render pass");
		}
	}
}
