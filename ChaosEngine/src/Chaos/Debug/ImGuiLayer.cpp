#include "chaospch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include "ImGUI/examples/imgui_impl_vulkan.h"
#include "ImGUI/examples/imgui_impl_glfw.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"
#include "Platform/Vulkan/VulkanRenderer.h"

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
		init_info.Instance = renderer.mInstance;
		init_info.PhysicalDevice = renderer.mPhysicalDevice;
		init_info.Device = renderer.mDevice;
		init_info.QueueFamily = renderer.FindQueueFamilies(renderer.mPhysicalDevice).graphicsFamily.value();
		init_info.Queue = renderer.mPresentQueue;
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = mDescriptorPool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = 2;
		init_info.ImageCount = 2;
		init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&init_info, renderpass);

		VkCommandBuffer commandBuffer = renderer.BeginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		renderer.EndSingleTimeCommands(commandBuffer);

		clear = { 0.0f, 0.0f, 0.0f, 0.0f };
	}

	void ImGuiLayer::OnDetach()
	{
		Application& app = Application::Get();
		VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(app.GetRenderer());
		vkDestroyRenderPass(renderer.mDevice, renderpass, nullptr);

		vkFreeCommandBuffers(renderer.mDevice, commandPool, static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());
		vkDestroyCommandPool(renderer.mDevice, commandPool, nullptr);

		// Resources to destroy when the program ends
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		vkDestroyDescriptorPool(renderer.mDevice, mDescriptorPool, nullptr);
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = false ;
		//ImGui::ShowDemoWindow(&show);-
		VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(Application::Get().GetRenderer());
		// FIXME-VIEWPORT: Select a default viewport
		const float DISTANCE = 10.0f;
		static int corner = 0;
		ImGuiIO& io = ImGui::GetIO();
		if (corner != -1)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 window_pos = ImVec2((corner & 1) ? (viewport->Pos.x + viewport->Size.x - DISTANCE) : (viewport->Pos.x + DISTANCE), (corner & 2) ? (viewport->Pos.y + viewport->Size.y - DISTANCE) : (viewport->Pos.y + DISTANCE));
			ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowViewport(viewport->ID);
		}
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		if (ImGui::Begin("FPS counter", &show, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			ImGui::Text("FPS: %f", 1 / mTime);
			ImGui::Text("Quads: %d", renderer.mTotalQuadsDrawn);
			ImGui::Text("Draw calls: %d", renderer.mBuffers.size() + 1);

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

		uint32_t mImageIndex = renderer.mImageIndex;

		//Create command bufferr
		mCommandBuffers.resize(renderer.mSwapchainImageViews.size());

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.commandBufferCount = (uint32_t)mCommandBuffers.size();
		vkAllocateCommandBuffers(renderer.mDevice, &commandBufferAllocateInfo, mCommandBuffers.data());

		frameBuffers.resize(renderer.mSwapchainImageViews.size());

		//Create frame buffers
		for (size_t i = 0; i < renderer.mSwapchainImageViews.size(); ++i)
		{
			VkImageView attachments[] = {
				renderer.mSwapchainImageViews[i]
			};
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderpass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = renderer.mSwapchainExtent.width;
			framebufferInfo.height = renderer.mSwapchainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(renderer.mDevice, &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS)
			{
				LOGCORE_ERROR("IMGUI: failed to create framebuffer!");
			}
		}

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		for (size_t i = 0; i < mCommandBuffers.size(); ++i)
		{
			VkRenderPassBeginInfo renderpassinfo = {};
			renderpassinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderpassinfo.renderPass = renderpass;
			renderpassinfo.framebuffer = frameBuffers[i];
			renderpassinfo.renderArea.extent.width = renderer.mSwapchainExtent.width;
			renderpassinfo.renderArea.extent.height = renderer.mSwapchainExtent.height;
			renderpassinfo.clearValueCount = 1;
			renderpassinfo.pClearValues = &clear;

			vkBeginCommandBuffer(mCommandBuffers[i], &beginInfo);

			vkCmdBindPipeline(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer.mGraphicsPipeline);
			vkCmdBeginRenderPass(mCommandBuffers[i], &renderpassinfo, VK_SUBPASS_CONTENTS_INLINE);
			


			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mCommandBuffers[i]);

			vkCmdEndRenderPass(mCommandBuffers[i]);
			vkEndCommandBuffer(mCommandBuffers[i]);
		}
		renderer.SetImGuiCommandBuffer(mCommandBuffers);	
		renderer.SetImGuiCommandPool(&commandPool);
		renderer.SetImGuiFramebuffer(&frameBuffers);
		renderer.mRenderingGUI = true;
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

		if (vkCreateDescriptorPool(renderer.mDevice, &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to allocate descriptor pool!");
		}


		//Create command pool
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = renderer.FindQueueFamilies(renderer.mPhysicalDevice).graphicsFamily.value();
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(renderer.mDevice, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
			LOGCORE_ERROR("IMGUI: Could not create graphics command pool");
		}

		VkAttachmentDescription attachment = {};
		attachment.format = renderer.mSwapchainImageFormat;
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

		if (vkCreateRenderPass(renderer.mDevice, &info, nullptr, &renderpass) != VK_SUCCESS) {
			LOGCORE_ERROR("IMGUI: Could not create ImGui's render pass");
		}
	}
}
