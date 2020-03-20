#include "chaospch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include "Chaos/Debug/ImGuiBuild.cpp"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "Vulkan/Include/vulkan/vulkan.h"
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
		Renderer& renderer = app.GetRenderer();

		device = renderer.GetLogicalDevice();

		VulkanInit();

		// Setup Platform/Renderer bindings		
		ImGui_ImplGlfw_InitForVulkan(window, true);

		// Setup Platform/Renderer bindings

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = renderer.GetInstance();
		init_info.PhysicalDevice = renderer.GetPhysicalDevice();
		init_info.Device = renderer.GetLogicalDevice();
		init_info.QueueFamily = renderer.FindQueueFamilies(renderer.GetPhysicalDevice()).graphicsFamily.value();
		init_info.Queue = renderer.GetQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = descriptorPool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = 2;
		init_info.ImageCount = 2;
		init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&init_info, renderpass);

		VkCommandBuffer commandBuffer = renderer.BeginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		renderer.EndSingleTimeCommands(commandBuffer);

		clear = { 0.0f, 0.0f, 0.0f, 0.f };
	}

	void ImGuiLayer::OnDetach()
	{

		vkDestroyRenderPass(device, renderpass, nullptr);

		vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		vkDestroyCommandPool(device, commandPool, nullptr);

		// Resources to destroy when the program ends
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());
		Renderer& renderer = app.GetRenderer();
		//Rendering			   
		ImGui::Render();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		size_t currentFrame = renderer.GetCurrentFrame();

		uint32_t imageIndex = renderer.GetCurrentImageIndex();

		//Create command bufferr
		commandBuffers.resize(renderer.GetSwapchainImageViews().size());

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();
		vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data());		

		frameBuffers.resize(renderer.GetSwapchainImageViews().size());

		//Create frame buffers
		for (size_t i = 0; i < renderer.GetSwapchainImageViews().size(); ++i)
		{
			VkImageView attachments[] = {
				renderer.GetSwapchainImageViews()[i]
			};
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderpass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = renderer.GetSwapChainExtent().width;
			framebufferInfo.height = renderer.GetSwapChainExtent().height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS)
			{
				LOGCORE_ERROR("IMGUI: failed to create framebuffer!");
			}
		}


		
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		for (size_t i = 0; i < commandBuffers.size(); ++i)
		{
			VkRenderPassBeginInfo renderpassinfo = {};
			renderpassinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderpassinfo.renderPass = renderpass;
			renderpassinfo.framebuffer = frameBuffers[i];
			renderpassinfo.renderArea.extent.width = renderer.GetSwapChainExtent().width;
			renderpassinfo.renderArea.extent.height = renderer.GetSwapChainExtent().height;
			renderpassinfo.clearValueCount = 1;
			renderpassinfo.pClearValues = &clear;

			vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

			vkCmdBeginRenderPass(commandBuffers[i], &renderpassinfo, VK_SUBPASS_CONTENTS_INLINE);

			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);
			vkEndCommandBuffer(commandBuffers[i]);
		}
		renderer.SetImGuiCommandBuffer(commandBuffers);	
		renderer.SetImGuiCommandPool(&commandPool);
		renderer.SetImGuiFramebuffer(&frameBuffers);
	}

	void ImGuiLayer::VulkanInit()
	{
		ImGuiIO& io = ImGui::GetIO();

		Application& app = Application::Get();
		Renderer& renderer = app.GetRenderer();

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

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to allocate descriptor pool!");
		}


		//Create command pool
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = renderer.FindQueueFamilies(renderer.GetPhysicalDevice()).graphicsFamily.value();
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
			LOGCORE_ERROR("IMGUI: Could not create graphics command pool");
		}

		VkAttachmentDescription attachment = {};
		attachment.format = renderer.GetSwapchainFormat();
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

		if (vkCreateRenderPass(device, &info, nullptr, &renderpass) != VK_SUCCESS) {
			LOGCORE_ERROR("IMGUI: Could not create ImGui's render pass");
		}


		/*
		// Create font texture
		unsigned char* fontData;
		int texWidth, texHeight;
		io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
		VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);

		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageInfo.extent.width = texWidth;
		imageInfo.extent.height = texHeight;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VK_CHECK_RESULT(vkCreateImage(device, &imageInfo, nullptr, &fontImage));
		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(device, fontImage, &memReqs);
		VkMemoryAllocateInfo memAllocInfo = {};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = renderer.FindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(device, &memAllocInfo, nullptr, &fontMemory));
		VK_CHECK_RESULT(vkBindImageMemory(device, fontImage, fontMemory, 0));

		// Staging buffers for font data upload
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;

		renderer.CreateBuffer(uploadSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingMemory);

		void* data;

		vkMapMemory(device, stagingMemory, 0, uploadSize, 0, &data);
		memcpy(data, fontData, uploadSize);
		vkUnmapMemory(device, stagingMemory);

		// Copy buffer data to font image
		renderer.TransitionImageLayout(fontImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		renderer.CopyBufferToImage(stagingBuffer, fontImage, texWidth, texHeight);

		//Destroy buffer
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingMemory, nullptr);

		fontView = renderer.CreateImageView(fontImage, VK_FORMAT_R8G8B8A8_UNORM);

		// Font texture Sampler
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		VK_CHECK_RESULT(vkCreateSampler(device, &samplerInfo, nullptr, &sampler));

		// Descriptor pool
		VkDescriptorPoolSize descriptorSize = {};
		descriptorSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSize.descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.pPoolSizes = &descriptorSize;
		descriptorPoolInfo.maxSets = 2;
		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool));

		// Descriptor set layout
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		layoutBinding.binding = 0;
		layoutBinding.descriptorCount = 1;
		layoutBinding.pImmutableSamplers = nullptr;
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
			layoutBinding
		};
		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
		descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.bindingCount = 1;
		descriptorLayoutInfo.pBindings = setLayoutBindings.data();
		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorLayoutInfo, nullptr, &descriptorSetLayout));

		// Descriptor set
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		allocInfo.descriptorSetCount = 1;
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet));
		VkDescriptorImageInfo fontDescriptor = {};
		fontDescriptor.sampler = sampler;
		fontDescriptor.imageView = fontView;
		fontDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.dstSet = descriptorSet;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.pImageInfo = &fontDescriptor;
		writeDescriptorSet.dstBinding = 0;

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
			writeDescriptorSet
		};
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

		// Pipeline layout
		// Push constants for UI rendering parameters
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstBlock);
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

		// Setup graphics pipeline for UI rendering
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
		inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyState.flags = 0;
		inputAssemblyState.primitiveRestartEnable = VK_FALSE;
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.cullMode = VK_CULL_MODE_NONE;
		rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.lineWidth = 1.0f;
		rasterizationState.depthBiasEnable = VK_FALSE;

		// Enable blending
		VkPipelineColorBlendAttachmentState blendAttachmentState{};
		blendAttachmentState.blendEnable = VK_TRUE;
		blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;


		VkPipelineColorBlendStateCreateInfo colorBlendState = {};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.logicOpEnable = VK_FALSE;
		colorBlendState.logicOp = VK_LOGIC_OP_COPY;
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = &blendAttachmentState;
		colorBlendState.blendConstants[0] = 0.0f;
		colorBlendState.blendConstants[1] = 0.0f;
		colorBlendState.blendConstants[2] = 0.0f;
		colorBlendState.blendConstants[3] = 0.0f;

		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilState.depthTestEnable = VK_FALSE;
		depthStencilState.depthWriteEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)renderer.GetSwapChainExtent().width;
		viewport.height = (float)renderer.GetSwapChainExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = renderer.GetSwapChainExtent();

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		std::vector<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.pDynamicStates = dynamicStateEnables.data();

		auto vertShaderCode = renderer.readFile("../ChaosEngine/shaders/ui.vert.spv");
		auto fragShaderCode = renderer.readFile("../ChaosEngine/shaders/ui.frag.spv");

		VkShaderModule vertShaderModule = renderer.CreateShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = renderer.CreateShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.renderPass = renderpass;
		pipelineCreateInfo.layout = pipelineLayout;
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineCreateInfo.pRasterizationState = &rasterizationState;
		pipelineCreateInfo.pColorBlendState = &colorBlendState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.pDepthStencilState = &depthStencilState;
		pipelineCreateInfo.pDynamicState = &dynamicState;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = shaderStages;

		// Vertex bindings an attributes based on ImGui vertex definition
		VkVertexInputBindingDescription vertexInputBinding;
		vertexInputBinding.stride = sizeof(ImDrawVert);
		vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputBinding.binding = 0;

		std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
			vertexInputBinding
		};

		VkVertexInputAttributeDescription posDescription;
		posDescription.binding = 0;
		posDescription.offset = offsetof(ImDrawVert, pos);
		posDescription.format = VK_FORMAT_R32G32_SFLOAT;
		posDescription.location = 0;

		VkVertexInputAttributeDescription uvDescription;
		uvDescription.binding = 1;
		uvDescription.offset = offsetof(ImDrawVert, uv);
		uvDescription.format = VK_FORMAT_R32G32_SFLOAT;
		uvDescription.location = 1;

		VkVertexInputAttributeDescription colorDescription;
		colorDescription.binding = 1;
		colorDescription.offset = offsetof(ImDrawVert, col);
		colorDescription.format = VK_FORMAT_R32G32_SFLOAT;
		colorDescription.location = 0;

		std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {
			posDescription, uvDescription, colorDescription
		};
		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
		vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
		vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
		vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

		pipelineCreateInfo.pVertexInputState = &vertexInputState;

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline));

		*/
	}
}
