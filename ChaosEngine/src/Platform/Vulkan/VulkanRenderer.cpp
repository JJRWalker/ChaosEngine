//This class contains functions for setting up a vulkan context, devices and pipelines. It also contains A function called "DrawFrame"
//that should be called at the end of the main update loop in application. This Class also has methods for adding quads to the render queue.
//This code was inspired by https://vulkan-tutorial.com/ and has been modified to better suit a game engine
//===============================================================NOTES======================================================================
//This class also works on the assumption that the user wont want to use a rendered frame for a viewport or anything else, as such there are 
//Areas excluded from release builds using precompiler macros, however this might change in the future as I can forsee users wanting to render
//say a cameras veiw to a texture on an entity but we'll cross that bridge when we come to it. 
//Another note, this class should be more abstracted into maybe interfaces such as "Buffer" and "Vertex" and then have platform derivatives from
//Those interfaces to keep this class lean, however I am not yet expirenced enough with render APIs to know how that abstraction should be done
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

#include "Platform/Vulkan/VulkanTexture.h"

#include "Chaos/DataTypes/Vec3.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec4.h"

#include "Chaos/Entity/Components/Camera.h"

#include <thread>
#include <functional>

//change to true if vulkan SDK is installed to recieve validation layer warnings, seems to have a bug currently where it will report that a descriptor set has not been updated
//even though it has and is rendering the image correctly.
#ifdef CHAOS_DEBUG
const bool enableValidationLayers = false;	
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
		vkDeviceWaitIdle(m_device);

		CleanUpSwapchain();

		vkDestroySampler(m_device, m_textureSampler, nullptr);

		vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);

		for (size_t i = 0; i < m_buffers.size(); ++i)
		{
			vkDestroyBuffer(m_device, m_buffers[i].VertexBuffer, nullptr);
			vkFreeMemory(m_device, m_buffers[i].VertexBufferMemory, nullptr);
			vkDestroyBuffer(m_device, m_buffers[i].IndexBuffer, nullptr);
			vkFreeMemory(m_device, m_buffers[i].IndexBufferMemory, nullptr);
		}


		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
		}

		vkResetCommandPool(m_device, m_commandPool, 0);

		if (m_renderingGUI)
		{
			vkResetCommandPool(m_device, *m_imGuiCommandPool, 0);
		}

		vkDestroyDevice(m_device, nullptr);

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkDestroyInstance(m_instance, nullptr);
	}

#pragma region DRAW QUAD
	//Called from outside the renderer class whenever the user wants to add anything to the render queue
	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Ref<Texture> texture)
	{
		DrawQuad(position, scale, Vec2(0, 0), Vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 1);
	}

	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Vec4& colour, Ref<Texture> texture)
	{
		DrawQuad(position, scale, Vec2(0, 0), colour, texture, 1);
	}

	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Vec4& colour, Ref<Texture> texture, float tilingFactor)
	{
		DrawQuad(position, scale, Vec2(0, 0), colour, texture, tilingFactor);
	}

	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Ref<Texture> texture, float tilingFactor)
	{
		DrawQuad(position, scale, Vec2(0, 0), Vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, tilingFactor);	}

	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Ref<SubTexture> subTexture)
	{
		DrawQuad(position, scale, Vec2(0,0), Vec4(1.0f, 1.0f, 1.0f, 1.0f), subTexture);

	}
	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Vec2& rotation, Ref<Texture> texture)
	{
		DrawQuad(position, scale, rotation, Vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 1);
	}
	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<Texture> texture)
	{
		DrawQuad(position, scale, rotation, colour, texture, 1);
	}

	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Vec2& rotation, Ref<SubTexture> subTexture)
	{
		DrawQuad(position, scale, rotation, Vec4(1.0f, 1.0f, 1.0f, 1.0f), subTexture);
	}

	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<Texture> texture, float tilingFactor)
	{
		PROFILED_FUNC();

		float imageIndex = (float)m_texturesToBind.size();
		for (size_t i = 0; i < m_texturesToBind.size(); ++i)
		{
			if (m_texturesToBind[i].get()->GetFilePath() == texture.get()->GetFilePath())
			{
				imageIndex = (float)i;
				break;
			}
		}


		glm::mat4 transform = glm::translate(glm::mat4(1), { position.X, position.Y, 0 })
			* glm::rotate(glm::mat4(1), glm::radians(rotation.X), { 0, 0, 1 })
			* glm::scale(glm::mat4(1), { scale.X, scale.Y, 0.0f });

		glm::vec4 vertexPositions[4] = { transform * QUAD_VERTEX_POSITIONS[0],
									   transform * QUAD_VERTEX_POSITIONS[1],
									   transform * QUAD_VERTEX_POSITIONS[2],
									   transform * QUAD_VERTEX_POSITIONS[3]};


		//pushing our vertexes to the back using the transformed vertex positions
		m_vertices.push_back(VulkanVertex{ {vertexPositions[0].x, vertexPositions[0].y, vertexPositions[0].z}, colour, Vec2(0.01f, tilingFactor), imageIndex });
		m_vertices.push_back(VulkanVertex{ {vertexPositions[1].x, vertexPositions[1].y, vertexPositions[1].z}, colour, Vec2(tilingFactor, tilingFactor), imageIndex });
		m_vertices.push_back(VulkanVertex{ {vertexPositions[2].x, vertexPositions[2].y, vertexPositions[2].z}, colour, Vec2(tilingFactor, 0.01f), imageIndex });
		m_vertices.push_back(VulkanVertex{ {vertexPositions[3].x, vertexPositions[3].y, vertexPositions[3].z}, colour, Vec2(0.01f, 0.01f), imageIndex });

		m_indices.push_back(0 + m_indOffset);
		m_indices.push_back(1 + m_indOffset);
		m_indices.push_back(2 + m_indOffset);
		m_indices.push_back(2 + m_indOffset);
		m_indices.push_back(3 + m_indOffset);
		m_indices.push_back(0 + m_indOffset);

		m_indOffset += 4;

		//if our image was not found, the index will be the same as the size of the vector, so push the texture to the back
		if (imageIndex == m_texturesToBind.size())
		{
			m_texturesToBind.push_back((Ref<VulkanTexture>&)texture);
		}

		m_renderCount++;

		if (m_texturesToBind.size() == MAX_TEXTURES_PER_DRAW || m_renderCount == MAX_OBJECTS_PER_DRAW)
		{
			m_buffers.resize(m_buffers.size() + 1);
			CreateBuffersAndClearResources(m_buffers.size() - 1);
		}
		m_debugInfo.TotalQuadsDrawn++;

	}

	void VulkanRenderer::DrawQuad(Vec2& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<SubTexture> subTexture)
	{
		float imageIndex = (float)m_texturesToBind.size();
		for (size_t i = 0; i < m_texturesToBind.size(); ++i)
		{
			if (m_texturesToBind[i].get()->GetFilePath() == subTexture.get()->GetFilePath())
			{
				imageIndex = (float)i;
				break;
			}
		}

		glm::mat4 transform = glm::translate(glm::mat4(1), { position.X, position.Y, 0 })
			* glm::rotate(glm::mat4(1), glm::radians(rotation.X), { 0, 0, 1 })
			* glm::scale(glm::mat4(1), { scale.X, scale.Y, 0.0f });

		glm::vec4 vertexPositions[4] = { transform * QUAD_VERTEX_POSITIONS[0],
									   transform * QUAD_VERTEX_POSITIONS[1],
									   transform * QUAD_VERTEX_POSITIONS[2],
									   transform * QUAD_VERTEX_POSITIONS[3] };


		m_vertices.push_back(VulkanVertex{ {vertexPositions[0].x, vertexPositions[0].y, vertexPositions[0].z}, colour, subTexture->GetTexCoords()[3], imageIndex });
		m_vertices.push_back(VulkanVertex{ {vertexPositions[1].x, vertexPositions[1].y, vertexPositions[1].z}, colour, subTexture->GetTexCoords()[2], imageIndex });
		m_vertices.push_back(VulkanVertex{ {vertexPositions[2].x, vertexPositions[2].y, vertexPositions[2].z}, colour, subTexture->GetTexCoords()[1], imageIndex });
		m_vertices.push_back(VulkanVertex{ {vertexPositions[3].x, vertexPositions[3].y, vertexPositions[3].z}, colour, subTexture->GetTexCoords()[0], imageIndex });

		m_indices.push_back(0 + m_indOffset);
		m_indices.push_back(1 + m_indOffset);
		m_indices.push_back(2 + m_indOffset);
		m_indices.push_back(2 + m_indOffset);
		m_indices.push_back(3 + m_indOffset);
		m_indices.push_back(0 + m_indOffset);

		m_indOffset += 4;

		if (imageIndex == m_texturesToBind.size())
		{
			m_texturesToBind.push_back((Ref<VulkanTexture>&)subTexture->GetMainTexture());
		}

		m_renderCount++;

		if (m_texturesToBind.size() == MAX_TEXTURES_PER_DRAW || m_renderCount == MAX_OBJECTS_PER_DRAW)
		{
			m_buffers.resize(m_buffers.size() + 1);
			CreateBuffersAndClearResources(m_buffers.size() - 1);
		}
		m_debugInfo.TotalQuadsDrawn++;
	}
#pragma endregion

	//Checks if the renderer currently has the texture in the specified file path stored, if it does it will return true and set old texture to that texture. Else returns false if none exists 
	bool VulkanRenderer::HasTexture(char* filePath, Ref<Texture> outTexture)
	{
		for (auto& t : m_texturesToBind)
		{
			if (t->GetFilePath() == filePath)
			{
				outTexture = t;
				return true;
			}
		}
		return false;
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
		CreateUniformBuffers();
		CreateDescriptorPool();
		//CreateDescriptorSet();
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
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames = m_validationLayers.data();

			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
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

		if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: could not create debug messenger");
		}
	}

	//Creating a platform angostic object to display on using window library in use, needs to be changed to use precompiler macros when porting to other systems
	void VulkanRenderer::CreateSurface() {
		if (glfwCreateWindowSurface(m_instance, (GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), nullptr, &m_surface) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create window surface!");
		}
	}

	//Selecting the correct graphics card, gives each device a score and picks the one that is most suitable for presenting graphics
	void VulkanRenderer::PickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			LOGCORE_ERROR("VULKAN: failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (IsDeviceSuitable(device)) {
				m_physicalDevice = device;
				break;
			}
		}

		if (m_physicalDevice == VK_NULL_HANDLE) {
			LOGCORE_ERROR("VULKAN: failed to find a suitable GPU!");
		}
	}

	//Creating logical device used for vulkan allocations, Called once upon renderer creation
	void VulkanRenderer::CreateLogicalDevice() {
		QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);

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

		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames = m_validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create logical device!");
		}

		vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
	}

	//Swapchain to be created on creation of renderer and whenever it is invalidated (most commonly on window resize)
	void VulkanRenderer::CreateSwapChain() {
		//Getting physical device swapchain support and capabilities
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_physicalDevice);

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
		createInfo.surface = m_surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
#if !CHAOS_RELEASE
		createInfo.imageUsage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
#else
		createInfo.imageUsage = VK_IMAGE_USAGE_SAMPLED_BIT; //VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
#endif

		QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
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

		if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to create swapchain");
		}

		vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
		m_swapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_swapchainImages.data());
		m_renderedFrames.resize(imageCount);
		m_renderedFramesMemory.resize(imageCount);

		m_swapchainImageFormat = surfaceFormat.format;
		m_swapchainExtent = extent;


		for (size_t i = 0; i < imageCount; ++i)
		{
			CreateImage(m_swapchainExtent.width, m_swapchainExtent.height, m_swapchainImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_renderedFrames[i], m_renderedFramesMemory[i]);
		}

	}

	//setting up the image views (still images) that will be presented to the surface, created upon renderer construction / creation
	void VulkanRenderer::CreateImageViews()
	{
		m_swapchainImageViews.resize(m_swapchainImages.size());
		m_renderedFrameViews.resize(m_renderedFrames.size());
		for (size_t i = 0; i < m_swapchainImages.size(); ++i)
		{
			m_swapchainImageViews[i] = CreateImageView(m_swapchainImages[i], m_swapchainImageFormat);
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_swapchainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_swapchainImageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapchainImageViews[i]) != VK_SUCCESS) {
				LOGCORE_ERROR("VULKAN: failed to create image views!");
			}


			m_renderedFrameViews[i] = CreateImageView(m_renderedFrames[i], m_swapchainImageFormat);
			VkImageViewCreateInfo frameViewsCreateInfo = {};
			frameViewsCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			frameViewsCreateInfo.image = m_renderedFrames[i];
			frameViewsCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			frameViewsCreateInfo.format = m_swapchainImageFormat;

			frameViewsCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			frameViewsCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			frameViewsCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			frameViewsCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			frameViewsCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			frameViewsCreateInfo.subresourceRange.baseMipLevel = 0;
			frameViewsCreateInfo.subresourceRange.levelCount = 1;
			frameViewsCreateInfo.subresourceRange.baseArrayLayer = 0;
			frameViewsCreateInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device, &frameViewsCreateInfo, nullptr, &m_renderedFrameViews[i]) != VK_SUCCESS) {
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
		samplerLayoutBinding.descriptorCount = 32;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create descriptor set layout!");
		}
	}

	//Renderpass created on renderer construction
	void VulkanRenderer::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = m_swapchainImageFormat;
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

		if (vkCreateRenderPass(m_device, &renderpassInfo, nullptr, &m_renderpass) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to create render pass!");
		}

		if (vkCreateRenderPass(m_device, &renderpassInfo, nullptr, &m_renderedRenderPass) != VK_SUCCESS)
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
		viewport.width = (float)m_swapchainExtent.width;
		viewport.height = (float)m_swapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = m_swapchainExtent;

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
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
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
		pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;

		if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
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
		pipelineInfo.layout = m_pipelineLayout;
		pipelineInfo.renderPass = m_renderpass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) 
		{
			LOGCORE_ERROR("VULKAN: failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
		vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
	}

	//Frame buffers only created on renderer construction
	void VulkanRenderer::CreateFrameBuffers()
	{
		m_swapchainframebuffers.resize(m_swapchainImageViews.size());
		m_renderedFrameBuffers.resize(m_renderedFrameViews.size());
		for (size_t i = 0; i < m_swapchainImageViews.size(); ++i)
		{
			VkImageView attachments[] = {
				m_swapchainImageViews[i]
			};
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_renderpass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_swapchainExtent.width;
			framebufferInfo.height = m_swapchainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapchainframebuffers[i]) != VK_SUCCESS)
			{
				LOGCORE_ERROR("VULKAN: failed to create framebuffer!");
			}

			VkImageView frameAttachments[] = {
				m_renderedFrameViews[i]
			};
			VkFramebufferCreateInfo renderedFrameframebufferInfo = {};
			renderedFrameframebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			renderedFrameframebufferInfo.renderPass = m_renderedRenderPass;
			renderedFrameframebufferInfo.attachmentCount = 1;
			renderedFrameframebufferInfo.pAttachments = frameAttachments;
			renderedFrameframebufferInfo.width = m_swapchainExtent.width;
			renderedFrameframebufferInfo.height = m_swapchainExtent.height;
			renderedFrameframebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_device, &renderedFrameframebufferInfo, nullptr, &m_renderedFrameBuffers[i]) != VK_SUCCESS)
			{
				LOGCORE_ERROR("VULKAN: failed to create framebuffer!");
			}

		}

	}

	//Creating a command pool to store a list of commands that will be executed on the DrawFrame function. Only created on construction
	//Cleared each frame after commands have been executed.
	void VulkanRenderer::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_physicalDevice);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to create command pool!");
		}
	}

	//Creating sampler on construction, determines how the image loaded should be modified from the raw data.
	//May at somepoint want to have multiple samplers depending on the desired effect for an image.
	void VulkanRenderer::CreateTextureSampler()
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 0;
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(m_device, &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to create texture sampler!");
		}
	}

	void VulkanRenderer::CreateIndexedBuffer(std::vector<VulkanVertex> vertices, std::vector<uint16_t> indices, BufferType type, size_t insertIndex)
	{
		if (m_buffers.size() < insertIndex + 1)
		{
			m_buffers.resize(insertIndex + 1);
		}
		//CREATING VERTEX BUFFER
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer vertexStagingBuffer;
		VkDeviceMemory vertexStagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexStagingBuffer, vertexStagingBufferMemory);

		void* vertData;
		vkMapMemory(m_device, vertexStagingBufferMemory, 0, bufferSize, 0, &vertData);
		memcpy(vertData, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_device, vertexStagingBufferMemory);

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

		CopyBuffer(vertexStagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(m_device, vertexStagingBuffer, nullptr);
		vkFreeMemory(m_device, vertexStagingBufferMemory, nullptr);

		//CREATING INDEX BUFFER
		bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer indexStagingBuffer;
		VkDeviceMemory indexStagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indexStagingBuffer, indexStagingBufferMemory);

		void* indData;
		vkMapMemory(m_device, indexStagingBufferMemory, 0, bufferSize, 0, &indData);
		memcpy(indData, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_device, indexStagingBufferMemory);

		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		CopyBuffer(indexStagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(m_device, indexStagingBuffer, nullptr);
		vkFreeMemory(m_device, indexStagingBufferMemory, nullptr);

		uint16_t highestInd = 0;

		//Getting highest indices and storing for future use
		for (size_t  i = indices.size() - 6; i < indices.size(); ++i)
		{
			if (indices[i] > highestInd)
				highestInd = indices[i];
		}

		Buffer buffer = { vertexBuffer, vertexBufferMemory, indexBuffer, indexBufferMemory, vertices.size(), indices.size(), highestInd, type };

		m_buffers[insertIndex] = buffer;
	}

	void VulkanRenderer::CreateBuffersAndClearResources(size_t insertIndex)
	{
		PROFILED_FUNC();
		CreateIndexedBuffer(m_vertices, m_indices, BufferType::Dynamic, insertIndex);
		m_buffers[insertIndex].TexturesToBind = m_texturesToBind;
		m_texturesToBind.clear();
		m_vertices.clear();
		m_indices.clear();
		m_indOffset = 0;
		m_renderCount = 0;
	}

	//SHOULD BE CALLED ON SCENE LOAD ONCE WE HAVE A MANIFEST OF ALL THE TEXTURES / SHADERS NEEDED IN THE SCENE
	void VulkanRenderer::CreateUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		m_uniformBuffers.resize(m_swapchainImages.size());	//Affording for 100 different textures, increadibly sub optimal. Increases memory footprint
		m_uniformBuffersMemory.resize(m_swapchainImages.size());

		for (size_t i = 0; i < m_swapchainImages.size(); ++i)
		{
			CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i], m_uniformBuffersMemory[i]);
		}
	}

	//Creating a pool to contain all the descriptor sets
	//SHOULD BE CALLED ON SCENE LOAD NOT EACH FRAME
	void VulkanRenderer::CreateDescriptorPool()
	{
		uint32_t numOfSets = 200;	//Allowing for a max of 200 x 32 textures currently per scene.
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

		if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to allocate descriptor pool!");
		}
	}

	//SHOULD ALSO BE CALLED ON SCENE LOAD TO AVOID CALLING EVERY FRAME
	void VulkanRenderer::CreateDescriptorSet()
	{
		m_descriptorSets.resize(m_buffers.size());
		std::vector<VkDescriptorSetLayout> layouts(m_buffers.size(), m_descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(m_buffers.size());
		allocInfo.pSetLayouts = layouts.data();

		if (vkAllocateDescriptorSets(m_device, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < m_buffers.size(); i++)
		{
			m_buffers[i].DescriptorIndex = i;
			m_buffers[i].TexturesLoaded = m_buffers[i].TexturesToBind.size();

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = m_uniformBuffers[0];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			std::vector<VkDescriptorImageInfo> imageInfo(m_buffers[i].TexturesToBind.size());

			//Itterating through the render queue and binding images to the descriptor set for texture array
			for (uint32_t imageIndex = 0; imageIndex < m_buffers[i].TexturesToBind.size(); ++imageIndex)
			{
				//Change to the indexed buffer at i
				imageInfo[imageIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo[imageIndex].imageView = m_buffers[i].TexturesToBind[imageIndex].get()->GetImageView();
				imageInfo[imageIndex].sampler = m_textureSampler;
			}

			VkWriteDescriptorSet descriptorWrites[2];

			descriptorWrites[0] = {};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1] = {};
			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = (uint32_t)m_buffers[i].TexturesToBind.size();
			descriptorWrites[1].pImageInfo = imageInfo.data();

			vkUpdateDescriptorSets(m_device, 2, descriptorWrites, 0, nullptr);
		}
	}

	//Helper for finding the right memory type 
	uint32_t VulkanRenderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

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
		PROFILED_FUNC();
		//Resizing vectors to contain the new buffers
		m_commandBuffers.resize(m_renderedFrameBuffers.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();


		if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) 
		{
			LOGCORE_ERROR("VULKAN: failed to allocate command buffers!");
			return;
		}


		for (size_t i = 0; i < m_renderedFrameBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				LOGCORE_ERROR("VULKAN: failed to begin recording command buffer!");
				return;
			}
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_renderpass;
			renderPassInfo.framebuffer = m_renderedFrameBuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = m_swapchainExtent;

			VkClearValue clearColor = { CLEAR_COLOR.X, CLEAR_COLOR.Y, CLEAR_COLOR.Z, CLEAR_COLOR.W };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

			//Itterates through RenderQueue and records commands, drawing all objects of the same texture together.
			for (uint32_t x = 0; x < m_buffers.size(); ++x)
			{
				//if a buffer is found that doesn't have all textures loaded, create descriptor sets
				if (m_buffers[x].TexturesLoaded != m_buffers[x].TexturesToBind.size())
				{
					if (m_descriptorSets.size() > 0)
					{
						vkFreeDescriptorSets(m_device, m_descriptorPool, (uint32_t)m_descriptorSets.size(), m_descriptorSets.data());
					}
					CreateDescriptorSet();
				}

				VkBuffer vertexBuffersToBind[] = { m_buffers[x].VertexBuffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(m_commandBuffers[i], 0, 1, vertexBuffersToBind, offsets);

				vkCmdBindIndexBuffer(m_commandBuffers[i], m_buffers[x].IndexBuffer, 0, VK_INDEX_TYPE_UINT16);

				vkCmdBindDescriptorSets(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[m_buffers[x].DescriptorIndex], 0, nullptr);

				vkCmdDrawIndexed(m_commandBuffers[i], static_cast<uint32_t>(m_buffers[x].IndexCount), 1, 0, 0, 0);
			}
			vkCmdEndRenderPass(m_commandBuffers[i]);
			if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS)
			{
				LOGCORE_ERROR("VULKAN: failed to record command buffer!");
			}

			//Code for copying image, not sure if we really need this if we're only rendering to the viewport in anything but release mode
			VkCommandBuffer command = BeginSingleTimeCommands();
			VkImageCopy copy = {};
			copy.extent.height = m_swapchainExtent.height;
			copy.extent.width = m_swapchainExtent.width;
			copy.srcOffset = { 0,0,0 };
			copy.dstOffset = { 0,0,0 };
			VkImageSubresourceLayers layers;
			layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			layers.mipLevel = 0;
			layers.baseArrayLayer = 0;
			layers.layerCount = 1;
			copy.srcSubresource = layers;
			copy.dstSubresource = layers;

			vkCmdCopyImage(command, m_renderedFrames[i], VK_IMAGE_LAYOUT_UNDEFINED, m_swapchainImages[i], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1, &copy);
			EndSingleTimeCommands(command);
		}
	}

	//Creating objects that allow the program to wait until an asyncronus task has been completed
	void VulkanRenderer::CreateSyncObjects()
	{
		//Resizing the sync object vectors to the max number of frames that can be in flight (the amount that will be asyncronusly prepared)
		m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		m_imagesInFlight.resize(m_swapchainImages.size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
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

		if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			LOGCORE_ERROR("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to allocate buffer memory!");
		}

		vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
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
		m_commandPoolMutex.lock();
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

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

		vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_graphicsQueue);

		vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
		m_commandPoolMutex.unlock();
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

		if (vkCreateImage(m_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to allocate image memory!");
		}
		vkBindImageMemory(m_device, image, imageMemory, 0);
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
		if (vkCreateImageView(m_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: failed to create texture image view!");
		}

		return imageView;
	}

	//Called once the swapchain is invalidated. Destroys all elements needed for swapchain creation
	void VulkanRenderer::CleanUpSwapchain()
	{
		for (auto framebuffer : m_swapchainframebuffers)
		{
			vkDestroyFramebuffer(m_device, framebuffer, nullptr);
		}

		vkFreeCommandBuffers(m_device, m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());

		vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
		vkDestroyRenderPass(m_device, m_renderpass, nullptr);

		for (auto imageView : m_swapchainImageViews) 
		{
			vkDestroyImageView(m_device, imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

		for (size_t i = 0; i < m_swapchainImages.size(); i++) 
		{
			vkDestroyBuffer(m_device, m_uniformBuffers[i], nullptr);
			vkFreeMemory(m_device, m_uniformBuffersMemory[i], nullptr);
		}


		for (auto framebuffer : m_renderedFrameBuffers)
		{
			vkDestroyFramebuffer(m_device, framebuffer, nullptr);
		}
		vkDestroyRenderPass(m_device, m_renderedRenderPass, nullptr);

		for (auto imageView : m_renderedFrameViews) 
		{
			vkDestroyImageView(m_device, imageView, nullptr);
		}

		for (size_t i = 0; i < m_renderedFrames.size(); i++) 
		{
			vkDestroyImage(m_device, m_renderedFrames[i], nullptr);
			vkFreeMemory(m_device, m_renderedFramesMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
	}

	//Called once the swapchain is invalidated. Calls all functions needed to destroy and recreate the swapchain
	void VulkanRenderer::RecreateSwapchain()
	{
		vkDeviceWaitIdle(m_device);

		CleanUpSwapchain();

		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFrameBuffers();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSet();
		CreateCommandBuffers();
		CreateSyncObjects();
	}

	//Updates all the UBOs maps memory for them. Resets vectors containing buffer memory and repopulates them
	void VulkanRenderer::UpdateUniformBuffers()
	{
		m_uniformBuffers.resize(m_swapchainImages.size());
		m_uniformBuffersMemory.resize(m_swapchainImages.size());
		for (size_t i = 0; i < m_swapchainImages.size(); ++i)
		{
			UniformBufferObject ubo = {};
			//ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			//ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			//ubo.proj = glm::ortho(-5.0f * (float)mSwapchainExtent.width / (float)mSwapchainExtent.height, 5.0f * (float)mSwapchainExtent.width / (float)mSwapchainExtent.height, 5.0f, -5.0f, -5.0f, 5.0f); //glm::perspective(glm::radians(45.0f), (float)swapchainExtent.width / (float)swapchainExtent.height, 0.1f, 1000.0f);

			
			ubo.view = Application::Get().GetMainCamera()->GetView();
			ubo.model = Application::Get().GetMainCamera()->GetModel();
			ubo.proj = Application::Get().GetMainCamera()->GetProjection();

			void* data;
			vkMapMemory(m_device, m_uniformBuffersMemory[i], 0, sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(m_device, m_uniformBuffersMemory[i]);
		}
	}
	


	//Draw called once every game loop to display the data passed to it that loop
	void VulkanRenderer::DrawFrame()
	{
		PROFILED_FUNC();
		if (m_vertices.size() > 0)
			CreateBuffersAndClearResources(m_buffers.size());

		m_debugInfo.NumOfDrawCalls = m_buffers.size();

		//DRAW
		m_waitingOnFences = true;
		vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);
		m_waitingOnFences = false;

		VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_imageIndex);

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
		if (m_imagesInFlight[m_imageIndex] != VK_NULL_HANDLE)
		{
			vkWaitForFences(m_device, 1, &m_imagesInFlight[m_imageIndex], VK_TRUE, UINT64_MAX);
		}

		m_imagesInFlight[m_imageIndex] = m_inFlightFences[m_currentFrame];

		//Adding the command buffers to the submit buffer, if there is an imgui buffer also attach imgui command buffers for drawing UI
		//Potential change: add check to see if a bool for rendering UI is true, would allow for toggling UI on and off. Could be done at a higher level
		std::vector<VkCommandBuffer> submitCommandBuffers = { m_commandBuffers[m_imageIndex] };
		if (m_imGuiCommandBuffers.size() > 0 && m_renderingGUI)
		{
			submitCommandBuffers.push_back(m_imGuiCommandBuffers[m_imageIndex]);
		}
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = (uint32_t)submitCommandBuffers.size();
		submitInfo.pCommandBuffers = submitCommandBuffers.data();

		VkSemaphore signalSemaphore[] = { m_renderFinishedSemaphores[m_currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphore;

		vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);

		if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphore;

		VkSwapchainKHR swapChains[] = { m_swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_imageIndex;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

		//if the swapchain has been invalidated or the frame buffer has been resized, then recreate it
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
		{
			m_framebufferResized = false;
			RecreateSwapchain();
			return;
		}
		else if (result != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to present swapchain image");
		}

		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		vkQueueWaitIdle(m_presentQueue);	//Waiting for the queue to be idle before clean up


		//Cleaning up resources after rendering a frame
		vkFreeCommandBuffers(m_device, m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());

		if (m_renderingGUI)
		{
			vkFreeCommandBuffers(m_device, *m_imGuiCommandPool, static_cast<uint32_t>(m_imGuiCommandBuffers.size()), m_imGuiCommandBuffers.data());
			for (auto framebuffer : *m_imGuiFrameBuffer) {
				vkDestroyFramebuffer(m_device, framebuffer, nullptr);
			}
		}

		//TODO: change to only free dynamic buffers
		for (size_t i = 0; i < m_buffers.size(); ++i)
		{
			vkDestroyBuffer(m_device, m_buffers[i].VertexBuffer, nullptr);
			vkFreeMemory(m_device, m_buffers[i].VertexBufferMemory, nullptr);
			vkDestroyBuffer(m_device, m_buffers[i].IndexBuffer, nullptr);
			vkFreeMemory(m_device, m_buffers[i].IndexBufferMemory, nullptr);
		}

		m_buffers.clear(); //TODO: change to only clear dynamic buffers
		m_imGuiCommandBuffers.clear();
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

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.Capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.Formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.PresentModes.data());
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

		std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

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
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

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

		for (const char* layerName : m_validationLayers) {
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
		if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			LOGCORE_ERROR("VULKAN: could not create shader module!");
		}

		return shaderModule;
	}
};
