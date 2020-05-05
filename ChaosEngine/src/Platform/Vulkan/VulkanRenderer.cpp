//This class contains functions for setting up a vulkan context, devices and pipelines. It also contains A function called "DrawFrame"
//that should be called at the end of the main update loop in application. This Class also has methods for adding quads to the render queue.
//This code was inspired by https://vulkan-tutorial.com/ and has been modified to better suit a game engine

#include "chaospch.h"
#include "VulkanRenderer.h"
#include "Chaos/Core/Application.h"
#include <GLFW/glfw3.h>

#include <GLM/glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <fstream>
#include <filesystem>
#include <chrono>

#include "Chaos/Renderer/Quad.h"
#include "Chaos/Renderer/Vertex.h"

#include <thread>
#include <functional>

#ifdef CHAOS_DEBUG
const bool enableValidationLayers = true;	//change to true if vulkan SDK is installed to recieve validation layer warnings
#else
const bool enableValidationLayers = false;
#endif


namespace Chaos
{
	VulkanRenderer::VulkanRenderer()
	{
		InitVulkan();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		vkDeviceWaitIdle(mDevice);

		CleanUpSwapchain();

		vkDestroySampler(mDevice, textureSampler, nullptr);

		vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);

		for (int i = 0; i < mIndexBuffers.size(); ++i)
		{
			vkDestroyBuffer(mDevice, mIndexBuffers[i], nullptr);
			vkFreeMemory(mDevice, mIndexBuffersMemory[i], nullptr);
		}

		for (int i = 0; i < mVertexBuffers.size(); ++i)
		{
			vkDestroyBuffer(mDevice, mVertexBuffers[i], nullptr);
			vkFreeMemory(mDevice, mVertexBuffersMemory[i], nullptr);
		}


		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
		}

		vkResetCommandPool(mDevice, mCommandPool, 0);

		if (mRenderingGUI)
		{
			vkResetCommandPool(mDevice, *mImGuiCommandPool, 0);
		}

		vkDestroyDevice(mDevice, nullptr);

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
		vkDestroyInstance(mInstance, nullptr);
	}

	VkResult VulkanRenderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VulkanRenderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT mDebugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, mDebugMessenger, pAllocator);
		}
	}

	//All functions that need to be called when a vulkan instance is set up 
	void VulkanRenderer::InitVulkan() {
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();
		CreateFrameBuffers();
		CreateCommandPool();
		CreateTextureSampler();
		//CreateVertexBuffers();
		//CreateIndexBuffers();
		CreateUniformBuffers();
		CreateDescriptorPool();
		//CreateDescriptorSets();
		//CreateCommandBuffers();
		CreateSyncObjects();
	}

	//Setting up an instance of vulkan to create a logical device, Only called once upon construction of the renderer
	void VulkanRenderer::CreateInstance() {
		if (enableValidationLayers && !CheckValidationLayerSupport()) {
			LOGCORE_ERROR("VULKAN: validation layers requested, but not available!");
		}

		//Passing information about the application
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Chaos Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Chaos Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		//Enabling debug extensions specified in header
		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
			createInfo.ppEnabledLayerNames = mValidationLayers.data();

			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create instance!");
		}
	}

	void VulkanRenderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	void VulkanRenderer::SetupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: could not create debug messenger");
		}
	}

	//Creating a platform angostic object to display on using window library in use, needs to be changed to use precompiler macros when porting to other systems
	void VulkanRenderer::CreateSurface() {
		if (glfwCreateWindowSurface(mInstance, (GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), nullptr, &mSurface) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create window surface!");
		}
	}

	//Selecting the correct graphics card, gives each device a score and picks the one that is most suitable for presenting graphics
	void VulkanRenderer::PickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			LOGCORE_ERROR("VULKAN: failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (IsDeviceSuitable(device)) {
				mPhysicalDevice = device;
				break;
			}
		}

		if (mPhysicalDevice == VK_NULL_HANDLE) {
			LOGCORE_ERROR("VULKAN: failed to find a suitable GPU!");
		}
	}

	//Creating logical device used for vulkan allocations, Called once upon renderer creation
	void VulkanRenderer::CreateLogicalDevice() {
		QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(mDeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
			createInfo.ppEnabledLayerNames = mValidationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create logical device!");
		}

		vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
		vkGetDeviceQueue(mDevice, indices.presentFamily.value(), 0, &mPresentQueue);
	}

	//Swapchain to be created on creation of renderer and whenever it is invalidated (most commonly on window resize)
	void VulkanRenderer::CreateSwapChain() {
		//Getting physical device swapchain support and capabilities
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(mPhysicalDevice);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) {
			imageCount = swapChainSupport.Capabilities.maxImageCount;
		}

		//using capbablilites to fill in parameters needed to create the swapchain
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mSurface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapchain) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to create swapchain");
		}

		vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, nullptr);
		mSwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, mSwapchainImages.data());

		mSwapchainImageFormat = surfaceFormat.format;
		mSwapchainExtent = extent;
	}

	//setting up the image views (still images) that will be presented to the surface, created upon renderer construction / creation
	void VulkanRenderer::CreateImageViews()
	{
		mSwapchainImageViews.resize(mSwapchainImages.size());
		for (size_t i = 0; i < mSwapchainImages.size(); ++i)
		{
			mSwapchainImageViews[i] = CreateImageView(mSwapchainImages[i], mSwapchainImageFormat);
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = mSwapchainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = mSwapchainImageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(mDevice, &createInfo, nullptr, &mSwapchainImageViews[i]) != VK_SUCCESS) {
				LOGCORE_ERROR("VULKAN: failed to create image views!");
			}
		}
	}

	//specifying a descriptor set layout, done upon renderer creation
	void VulkanRenderer::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;



		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(mDevice, &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create descriptor set layout!");
		}
	}

	//Renderpass created on renderer construction
	void VulkanRenderer::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = mSwapchainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachRef = {};
		colorAttachRef.attachment = 0;
		colorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderpassInfo = {};
		renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpassInfo.attachmentCount = 1;
		renderpassInfo.pAttachments = &colorAttachment;
		renderpassInfo.subpassCount = 1;
		renderpassInfo.pSubpasses = &subpass;
		renderpassInfo.dependencyCount = 1;
		renderpassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(mDevice, &renderpassInfo, nullptr, &mRenderPass) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to create render pass!");
		}
	}

	//Graphics pipeline currently only created on construction, currently hardcoded to use one shader. Cannot load a new shader needs to be fixed at a later date
	void VulkanRenderer::CreateGraphicsPipeline()
	{
		auto vertShaderCode = readFile("../chaosengine/shaders/vert.spv");
		auto fragShaderCode = readFile("../chaosengine/shaders/frag.spv");

		VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

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

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		auto bindingDescription = VulkanVertex::GetBindingDescriptions();
		auto attributeDescriptions = VulkanVertex::GetAttributeDescriptions();

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)mSwapchainExtent.width;
		viewport.height = (float)mSwapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = mSwapchainExtent;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.blendEnable = VK_TRUE;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;

		if (vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = mPipelineLayout;
		pipelineInfo.renderPass = mRenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline) != VK_SUCCESS) 
		{
			LOGCORE_ERROR("VULKAN: failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(mDevice, fragShaderModule, nullptr);
		vkDestroyShaderModule(mDevice, vertShaderModule, nullptr);
	}

	//Frame buffers only created on renderer construction
	void VulkanRenderer::CreateFrameBuffers()
	{
		mSwapchainframebuffers.resize(mSwapchainImageViews.size());

		for (size_t i = 0; i < mSwapchainImageViews.size(); ++i)
		{
			VkImageView attachments[] = {
				mSwapchainImageViews[i]
			};
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = mRenderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = mSwapchainExtent.width;
			framebufferInfo.height = mSwapchainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(mDevice, &framebufferInfo, nullptr, &mSwapchainframebuffers[i]) != VK_SUCCESS)
			{
				LOGCORE_ERROR("VULKAN: failed to create framebuffer!");
			}
		}

	}

	//Creating a command pool to store a list of commands that will be executed on the DrawFrame function. Only created on construction
	//Cleared each frame after commands have been executed.
	void VulkanRenderer::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(mPhysicalDevice);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to create command pool!");
		}
	}

	//DEPRICATED, TO BE REMOVED. Vulkan image creation is now handled on the VulkanTexture itself. Image and other data stored there aswell
	void VulkanRenderer::CreateTextureImage(Texture* tex)
	{
		int texWidth, texHeight, texChannels;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VkImage image;
		VkDeviceMemory memory;
		VkImageView view;
		//if the texture does not have data, load data, otherwise use pixel data stored on the texture object
		if (tex->GetData() == NULL)
		{
			if (!std::filesystem::exists(tex->GetFilePath()))
			{
				LOGCORE_WARN("VULKAN: could not open file {0}", tex->GetFilePath());
				tex->SetFilePath("../Game/textures/blank.png");
			}

			stbi_uc* pixels = stbi_load(tex->GetFilePath(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			VkDeviceSize imageSize = texWidth * texHeight * 4;

			if (!pixels) {
				LOGCORE_ERROR("VULKAN: failed to load texture image!");
			}

			CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(mDevice, stagingBufferMemory, 0, imageSize, 0, &data);
			memcpy(data, pixels, static_cast<size_t>(imageSize));
			vkUnmapMemory(mDevice, stagingBufferMemory);

			stbi_image_free(pixels);
		}
		else
		{
			texWidth = tex->GetWidth();
			texHeight = tex->GetHeight();
			VkDeviceSize imageSize = texWidth * texHeight * 4;

			CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(mDevice, stagingBufferMemory, 0, imageSize, 0, &data);
			memcpy(data, tex->GetData(), static_cast<size_t>(imageSize));
			vkUnmapMemory(mDevice, stagingBufferMemory);
		}

		CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory);

		TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);

		view = CreateImageView(image, VK_FORMAT_R8G8B8A8_SRGB);

		VulkanTexture* loaded = new VulkanTexture(*(VulkanTexture*)tex);
	}

	//Creating sampler on construction, determines how the image loaded should be modified from the raw data.
	//May at somepoint want to have multiple samplers depending on the desired effect for an image.
	void VulkanRenderer::CreateTextureSampler()
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(mDevice, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to create texture sampler!");
		}
	}

	//Creating a buffer to load vertex data into, cleared and loaded into each time a draw command is written, created on rendeder construction
	void VulkanRenderer::CreateVertexBuffers(std::vector<VulkanVertex> vertices, size_t insertIndex)
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(mDevice, stagingBufferMemory);

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

		CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);

		mVertexBuffers[insertIndex] = vertexBuffer;
		mVertexBuffersMemory[insertIndex] = vertexBufferMemory;
	}

	//Creating an index buffer, stores list of indicies that point to locations in the vertex buffer to render, cleared and loaded to when a new command is written
	//created on renderer construction
	void VulkanRenderer::CreateIndexBuffers(std::vector<uint16_t> indices, size_t insertIndex)
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(mDevice, stagingBufferMemory);

		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);


		mIndexBuffers[insertIndex] = indexBuffer;
		mIndexBuffersMemory[insertIndex] = indexBufferMemory;
	}

	void VulkanRenderer::PopulateBuffers(size_t renderQueueIndex)
	{
		//updating vertex and index buffers using the indicies and vertcies from the render queue
		CreateVertexBuffers(mRenderQueue[renderQueueIndex].Vertices, renderQueueIndex);
		CreateIndexBuffers(mRenderQueue[renderQueueIndex].Indices, renderQueueIndex);
	}

	//SHOULD BE CALLED ON SCENE LOAD ONCE WE HAVE A MANIFEST OF ALL THE TEXTURES / SHADERS NEEDED IN THE SCENE
	void VulkanRenderer::CreateUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		mUniformBuffers.resize(mSwapchainImages.size() + 100);	//Affording for 100 different textures, increadibly sub optimal. Increases memory footprint
		mUniformBuffersMemory.resize(mSwapchainImages.size() + 100);

		for (size_t i = 0; i < mSwapchainImages.size() + 100; ++i)
		{
			CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mUniformBuffers[i], mUniformBuffersMemory[i]);
		}
	}

	//Creating a pool to contain all the descriptor sets
	//SHOULD BE CALLED ON SCENE LOAD NOT EACH FRAME
	void VulkanRenderer::CreateDescriptorPool()
	{
		uint32_t numOfSets = 100;	//Allowing for a max of 100 textures currently per scene.
		//This value could be changed when the scene is loaded for optimisation, however memory footprint for a descriptor set is small

		std::array<VkDescriptorPoolSize, 2> poolSizes = {};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = numOfSets;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = numOfSets;

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = numOfSets;

		if (vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to allocate descriptor pool!");
		}
	}

	//SHOULD ALSO BE CALLED ON SCENE LOAD TO AVOID CALLING EVERY FRAME
	void VulkanRenderer::CreateDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(mRenderQueue.size(), mDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = mDescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(mRenderQueue.size());
		allocInfo.pSetLayouts = layouts.data();

		mDescriptorSets.resize(mRenderQueue.size());
		if (vkAllocateDescriptorSets(mDevice, &allocInfo, mDescriptorSets.data()) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < mRenderQueue.size(); i++)
		{

			VulkanTexture& texture = *(VulkanTexture*)mRenderQueue[i].Texture;

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = mUniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = texture.GetImageView();
			imageInfo.sampler = textureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = mDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = mDescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(mDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	//Helper for finding the right memory type 
	uint32_t VulkanRenderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & props) == props) {
				return i;
			}
		}

		LOGCORE_ERROR("VULKAN: failed to find suitable memory type!");

		return NULL;
	}

	//Creating command buffers each frame, loads vertex, index and texture data from mRenderQueue and records commands to draw.
	void VulkanRenderer::CreateCommandBuffers()
	{
		//Resizing vectors to contain the new buffers
		mCommandBuffers.resize(mSwapchainframebuffers.size());
		//Resizing vertex and index buffers by the number to expect times the number of command buffers 
		mVertexBuffers.resize(mRenderQueue.size() * mCommandBuffers.size());
		mVertexBuffersMemory.resize(mRenderQueue.size() * mCommandBuffers.size());
		mIndexBuffers.resize(mRenderQueue.size() * mCommandBuffers.size());
		mIndexBuffersMemory.resize(mRenderQueue.size() * mCommandBuffers.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = mCommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)mCommandBuffers.size();


		if (vkAllocateCommandBuffers(mDevice, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS) 
		{
			LOGCORE_ERROR("VULKAN: failed to allocate command buffers!");
			return;
		}

		std::vector<std::thread*> threads;
		threads.resize(mRenderQueue.size());
		for (size_t x = 0; x < mRenderQueue.size(); ++x)
		{
			threads[x] = new std::thread(&VulkanRenderer::PopulateBuffers, this, x);
		}


		for (size_t x = 0; x < threads.size(); ++x)
		{
			threads[x]->join();
			delete threads[x];
		}


		//TO BE MOVED TO ON SCENE LOAD FUNC
		if (mDescriptorSets.size() != mRenderQueue.size())
		{
			if (mDescriptorSets.size() > 0)
			{
				vkFreeDescriptorSets(mDevice, mDescriptorPool, (uint32_t)mDescriptorSets.size(), mDescriptorSets.data());
			}
			CreateDescriptorSets();
		}

		for (size_t i = 0; i < mCommandBuffers.size(); i++) 
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(mCommandBuffers[i], &beginInfo) != VK_SUCCESS) 
			{
				LOGCORE_ERROR("VULKAN: failed to begin recording command buffer!");
				return;
			}
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = mRenderPass;
			renderPassInfo.framebuffer = mSwapchainframebuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = mSwapchainExtent;

			VkClearValue clearColor = { mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(mCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

			//Itterates through RenderQueue and records commands, drawing all objects of the same texture together.
			for (int x = 0; x < mRenderQueue.size(); ++x)
			{
				VkBuffer vertexBuffersToBind[] = { mVertexBuffers[x] };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(mCommandBuffers[i], 0, 1, vertexBuffersToBind, offsets);

				vkCmdBindIndexBuffer(mCommandBuffers[i], mIndexBuffers[x], 0, VK_INDEX_TYPE_UINT16);

				vkCmdBindDescriptorSets(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSets[x], 0, nullptr);

				vkCmdDrawIndexed(mCommandBuffers[i], static_cast<uint32_t>(mRenderQueue[x].Indices.size()), 1, 0, 0, 0);

			}
			vkCmdEndRenderPass(mCommandBuffers[i]);
			if (vkEndCommandBuffer(mCommandBuffers[i]) != VK_SUCCESS)
			{
				LOGCORE_ERROR("VULKAN: failed to record command buffer!");
			}
		}
	}

	//Creating objects that allow the program to wait until an asyncronus task has been completed
	void VulkanRenderer::CreateSyncObjects()
	{
		//Resizing the sync object vectors to the max number of frames that can be in flight (the amount that will be asyncronusly prepared)
		mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		mImagesInFlight.resize(mSwapchainImages.size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS)
			{
				LOGCORE_ERROR("VULKAN: failed to create synchronization objects for frame {0}!", i);
			}
		}
	}

	//Helper function to create VK buffers, takes in a reference to the buffer and buffer memory that it binds to the device
	void VulkanRenderer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(mDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			LOGCORE_ERROR("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(mDevice, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to allocate buffer memory!");
		}

		vkBindBufferMemory(mDevice, buffer, bufferMemory, 0);
	}

	//Helper function for copying buffers using single time commands
	void VulkanRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		EndSingleTimeCommands(commandBuffer);
	}

	//Helper function used for copying buffer to image using single time commands
	void VulkanRenderer::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		EndSingleTimeCommands(commandBuffer);
	}

	//Helper func that begins a single time command, allocates a command to the pool and starts recording
	VkCommandBuffer VulkanRenderer::BeginSingleTimeCommands()
	{
		commandPoolMutex.lock();
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = mCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	//Helper function that ends a single time command. Does not check to see if one was started yet.
	void VulkanRenderer::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(mGraphicsQueue);

		vkFreeCommandBuffers(mDevice, mCommandPool, 1, &commandBuffer);
		commandPoolMutex.unlock();
	}

	//Creates and binds an image to an image memory
	void VulkanRenderer::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(mDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(mDevice, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to allocate image memory!");
		}
		vkBindImageMemory(mDevice, image, imageMemory, 0);
	}

	//Translates the image format, used to make sending the image data to the GPU more performant
	void VulkanRenderer::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			LOGCORE_ERROR("VULKAN: unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		EndSingleTimeCommands(commandBuffer);
	}

	//Creates the image for the provided image and returns it. Called from inside VulkanTexture. Move in future.
	VkImageView VulkanRenderer::CreateImageView(VkImage image, VkFormat format)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(mDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create texture image view!");
		}

		return imageView;
	}

	//Called once the swapchain is invalidated. Destroys all elements needed for swapchain creation
	void VulkanRenderer::CleanUpSwapchain()
	{
		for (auto framebuffer : mSwapchainframebuffers)
		{
			vkDestroyFramebuffer(mDevice, framebuffer, nullptr);
		}

		vkFreeCommandBuffers(mDevice, mCommandPool, static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());

		vkDestroyPipeline(mDevice, mGraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
		vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

		for (auto imageView : mSwapchainImageViews) {
			vkDestroyImageView(mDevice, imageView, nullptr);
		}

		vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);

		for (size_t i = 0; i < mSwapchainImages.size(); i++) {
			vkDestroyBuffer(mDevice, mUniformBuffers[i], nullptr);
			vkFreeMemory(mDevice, mUniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);
	}

	//Called once the swapchain is invalidated. Calls all functions needed to destroy and recreate the swapchain
	void VulkanRenderer::RecreateSwapchain()
	{
		vkDeviceWaitIdle(mDevice);

		CleanUpSwapchain();

		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFrameBuffers();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
		CreateSyncObjects();
	}

	//Updates all the UBOs maps memory for them. Resets vectors containing buffer memory and repopulates them
	void VulkanRenderer::UpdateUniformBuffers()
	{
		mUniformBuffers.resize(mRenderQueue.size());
		mUniformBuffersMemory.resize(mRenderQueue.size());
		for (size_t i = 0; i < mRenderQueue.size(); ++i)
		{
			UniformBufferObject ubo = {};
			//ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			//ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.proj = glm::ortho(-5.0f * (float)mSwapchainExtent.width / (float)mSwapchainExtent.height, 5.0f * (float)mSwapchainExtent.width / (float)mSwapchainExtent.height, 5.0f, -5.0f, -5.0f, 5.0f); //glm::perspective(glm::radians(45.0f), (float)swapchainExtent.width / (float)swapchainExtent.height, 0.1f, 1000.0f);
			//ubo.proj[1][1] *= -1;

			void* data;
			vkMapMemory(mDevice, mUniformBuffersMemory[i], 0, sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(mDevice, mUniformBuffersMemory[i]);
		}
	}
	
	//Called from outside the renderer class whenever the user wants to add anything to the render queue
	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Texture* texture)
	{
		//Creates local vector to store all the verts for the quad using the position and scale given.
		std::vector<VulkanVertex> verts;
		verts.push_back(VulkanVertex{ Vec2((-1.f * scale.X / 2) + position.X, (-1.f * scale.Y / 2) + position.Y), Vec4(1, 1, 1, 1), Vec2(0.01f, texture->GetTilingFactor()) });
		verts.push_back(VulkanVertex{ Vec2((1.f * scale.X / 2) + position.X, (-1.f * scale.Y / 2) + position.Y), Vec4(1, 1, 1, 1), Vec2(texture->GetTilingFactor(), texture->GetTilingFactor()) });
		verts.push_back(VulkanVertex{ Vec2((1.f * scale.X / 2) + position.X, (1.f * scale.Y / 2) + position.Y), Vec4(1, 1, 1, 1), Vec2(texture->GetTilingFactor(), 0.01f) });
		verts.push_back(VulkanVertex{ Vec2((-1.f * scale.X / 2) + position.X, (1.f * scale.Y / 2) + position.Y), Vec4(1, 1, 1, 1), Vec2(0.01f, 0.01f) });

		std::vector<uint16_t> ind = {
			0,1,2,2,3,0
		};
	
		for (auto& d : mRenderQueue)
		{
			if (d.Texture->GetFilePath() == texture->GetFilePath())
			{
				uint16_t highestInd = 0;
				for (int i = 0; i < mRenderQueue[mRenderQueue.size() - 1].Indices.size(); ++i)
				{
					if (mRenderQueue[mRenderQueue.size() - 1].Indices[i] > highestInd)
					{
						highestInd = mRenderQueue[mRenderQueue.size() - 1].Indices[i];
					}
				}
				for (int i = 0; i < ind.size(); ++i)
				{
					ind[i] += highestInd + 1;
				}
				d.Indices.insert(d.Indices.begin(), ind.begin(), ind.end());
				d.Vertices.insert(d.Vertices.begin(), verts.begin(), verts.end());
				return;
			}
		}

		RenderData data = { verts, ind, texture };
		mRenderQueue.push_back(data);
	}

	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Vec4& colour, Texture* texture)
	{
		//Creates local vector to store all the verts for the quad using the position and scale given.
		std::vector<VulkanVertex> verts;
		verts.push_back(VulkanVertex{ Vec2((-1.f * scale.X / 2) + position.X, (-1.f * scale.Y / 2) + position.Y), colour, Vec2(0.01f, texture->GetTilingFactor()) });
		verts.push_back(VulkanVertex{ Vec2((1.f * scale.X / 2) + position.X, (-1.f * scale.Y / 2) + position.Y), colour, Vec2(texture->GetTilingFactor(), texture->GetTilingFactor()) });
		verts.push_back(VulkanVertex{ Vec2((1.f * scale.X / 2) + position.X, (1.f * scale.Y / 2) + position.Y),colour, Vec2(texture->GetTilingFactor(), 0.01f) });
		verts.push_back(VulkanVertex{ Vec2((-1.f * scale.X / 2) + position.X, (1.f * scale.Y / 2) + position.Y), colour, Vec2(0.01f, 0.01f) });

		std::vector<uint16_t> ind = {
			0,1,2,2,3,0
		};

		for (auto& d : mRenderQueue)
		{
			if (d.Texture->GetFilePath() == texture->GetFilePath())
			{
				uint16_t highestInd = 0;
				for (int i = 0; i < mRenderQueue[mRenderQueue.size() - 1].Indices.size(); ++i)
				{
					if (mRenderQueue[mRenderQueue.size() - 1].Indices[i] > highestInd)
					{
						highestInd = mRenderQueue[mRenderQueue.size() - 1].Indices[i];
					}
				}
				for (int i = 0; i < ind.size(); ++i)
				{
					ind[i] += highestInd + 1;
				}
				d.Indices.insert(d.Indices.begin(), ind.begin(), ind.end());
				d.Vertices.insert(d.Vertices.begin(), verts.begin(), verts.end());
				return;
			}
		}

		RenderData data = { verts, ind, texture };
		mRenderQueue.push_back(data);
	}

	//Draw called once every game loop to display the data passed to it that loop
	void VulkanRenderer::DrawFrame()
	{
		if (mRenderQueue.size() > 0)
		{
			//DRAW
			mWaitingOnFences = true;
			vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);
			vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);
			mWaitingOnFences = false;

			VkResult result = vkAcquireNextImageKHR(mDevice, mSwapchain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &mImageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				RecreateSwapchain();
				return;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			{
				LOGCORE_ERROR("VULKAN: failed to aquire swapchain image!");
			}

			UpdateUniformBuffers();
			CreateCommandBuffers();

			//Check if a previous frame is using the same image
			if (mImagesInFlight[mImageIndex] != VK_NULL_HANDLE)
			{
				vkWaitForFences(mDevice, 1, &mImagesInFlight[mImageIndex], VK_TRUE, UINT64_MAX);
			}

			mImagesInFlight[mImageIndex] = mInFlightFences[mCurrentFrame];

			//Adding the command buffers to the submit buffer, if there is an imgui buffer also attach imgui command buffers for drawing UI
			//Potential change: add check to see if a bool for rendering UI is true, would allow for toggling UI on and off. Could be done at a higher level
			std::vector<VkCommandBuffer> submitCommandBuffers = { mCommandBuffers[mImageIndex] };
			if (mImGuiCommandBuffers.size() > 0 && mRenderingGUI)
			{
				submitCommandBuffers.push_back(mImGuiCommandBuffers[mImageIndex]);
			}
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;

			submitInfo.commandBufferCount = (uint32_t)submitCommandBuffers.size();
			submitInfo.pCommandBuffers = submitCommandBuffers.data();

			VkSemaphore signalSemaphore[] = { mRenderFinishedSemaphores[mCurrentFrame] };
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphore;

			vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

			if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS)
			{
				LOGCORE_ERROR("VULKAN: failed to submit draw command buffer!");
			}

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphore;

			VkSwapchainKHR swapChains[] = { mSwapchain };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &mImageIndex;
			presentInfo.pResults = nullptr;

			result = vkQueuePresentKHR(mPresentQueue, &presentInfo);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized)
			{
				mFramebufferResized = false;
				RecreateSwapchain();
				return;
			}
			else if (result != VK_SUCCESS)
			{
				LOGCORE_ERROR("VULKAN: failed to present swapchain image");
			}

			mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
			vkQueueWaitIdle(mPresentQueue);	//Waiting for the queue to be idle before clean up


			//Cleaning up resources after rendering a frame
			vkFreeCommandBuffers(mDevice, mCommandPool, static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());

			if (mRenderingGUI)
			{
				vkFreeCommandBuffers(mDevice, *mImGuiCommandPool, static_cast<uint32_t>(mImGuiCommandBuffers.size()), mImGuiCommandBuffers.data());
				for (auto framebuffer : *mImGuiFrameBuffer) {
					vkDestroyFramebuffer(mDevice, framebuffer, nullptr);
				}
			}

			for (int i = 0; i < mVertexBuffers.size(); ++i)
			{
				vkDestroyBuffer(mDevice, mVertexBuffers[i], nullptr);
				vkFreeMemory(mDevice, mVertexBuffersMemory[i], nullptr);
			}

			for (int i = 0; i < mIndexBuffers.size(); ++i)
			{
				vkDestroyBuffer(mDevice, mIndexBuffers[i], nullptr);
				vkFreeMemory(mDevice, mIndexBuffersMemory[i], nullptr);
			}

			//deleting all pointers before clearing the list of rendered objects
			mRenderQueue.clear();
			mIndicies.clear();
			mVertices.clear();
			mIndexBuffers.clear();
			mIndexBuffersMemory.clear();
			mVertexBuffers.clear();
			mVertexBuffersMemory.clear();
			mImGuiCommandBuffers.clear();
			mRenderQueue.clear();
		}

	}

	VkSurfaceFormatKHR VulkanRenderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR VulkanRenderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanRenderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = { Application::Get().GetWindow().GetWidth(),Application::Get().GetWindow().GetHeight() };

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	SwapChainSupportDetails VulkanRenderer::QuerySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.Capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.Formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.PresentModes.data());
		}

		return details;
	}

	bool VulkanRenderer::IsDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = FindQueueFamilies(device);

		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	bool VulkanRenderer::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(mDeviceExtensions.begin(), mDeviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	QueueFamilyIndices VulkanRenderer::FindQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	std::vector<const char*> VulkanRenderer::GetRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	bool VulkanRenderer::CheckValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : mValidationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		LOGCORE_WARN("VULKAN: validation layer: {0}", pCallbackData->pMessage);

		return VK_FALSE;
	}
	std::vector<char> VulkanRenderer::readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!std::filesystem::exists(filename))
			LOGCORE_ERROR("VULKAN: could not open file {0}", filename);


		if (!file.is_open()) {
			LOGCORE_ERROR("VULKAN: could not open file {0}", filename);
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		//LOGCORE_INFO("VULKAN: File read {0}", filename);

		return buffer;
	}
	VkShaderModule VulkanRenderer::CreateShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(mDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: could not create shader module!");
		}

		return shaderModule;
	}
};
