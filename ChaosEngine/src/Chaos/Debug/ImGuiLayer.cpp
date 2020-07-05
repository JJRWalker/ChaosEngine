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
#if !CHAOS_RELEASE		
		m_veiwportImageId = (ImTextureID)ImGui_ImplVulkan_AddTexture(renderer.m_textureSampler, renderer.m_renderedFrameViews[renderer.m_currentFrame], VK_IMAGE_LAYOUT_UNDEFINED);
#endif
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

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = false;
		//ImGui::ShowDemoWindow(&show);
		VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(Application::Get().GetRenderer());

#if !CHAOS_RELEASE
		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit")) Chaos::Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::Text("FPS: %f", 1 / mTime);
		ImGui::Text("Quads: %d", renderer.m_totalQuadsDrawn);
		ImGui::Text("Draw calls: %d", renderer.m_buffers.size() + 1);
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("viewport");		

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		if (m_veiwportImageId != nullptr)
		{
			ImGui::Image(m_veiwportImageId, { viewportSize.x, viewportSize.y}, ImVec2{ 0, -1 }, ImVec2{ 1, 0 });
		}
		ImGui::End();
		ImGui::PopStyleVar();
#endif
		// FIXME-VIEWPORT: Select a default viewport
		const float DISTANCE = 10.0f;
		static int corner = 2;
		if (corner != -1)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 window_pos = ImVec2((corner & 1) ? (viewport->Pos.x + viewport->Size.x - DISTANCE) : (viewport->Pos.x + DISTANCE), (corner & 2) ? (viewport->Pos.y + viewport->Size.y - DISTANCE) : (viewport->Pos.y + DISTANCE));
			ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowViewport(viewport->ID);
		}
		//ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		if (ImGui::Begin("FPS counter", &show, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			ImGui::Text("FPS: %f", 1 / mTime);
			ImGui::Text("Quads: %d", renderer.m_totalQuadsDrawn);
			ImGui::Text("Draw calls: %d", renderer.m_buffers.size() + 1);

			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
				if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
				if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
				if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
				if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
				if (&show && ImGui::MenuItem("Close")) show = false;
				ImGui::EndPopup();
			}
		}
		ImGui::End();


		renderer.m_totalQuadsDrawn = 0;
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
