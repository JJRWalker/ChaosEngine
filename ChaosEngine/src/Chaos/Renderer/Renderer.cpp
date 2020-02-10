#include "chaospch.h"

#include "Renderer.h"
#include <map>
#include <optional>

/*
#ifndef CHAOS_DEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
*/

namespace Chaos
{
	const bool enableValidationLayers = false;
	Renderer::Renderer()
	{
		VulkanInit();
	}

	Renderer::~Renderer()
	{
		vkDestroyInstance(vkInstance, nullptr);
	}

	void Renderer::VulkanInit()
	{
		//CREATING CONTEXT///////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		COREASSERT(enableValidationLayers && !checkValidationLayerSupport(), "VULKAN: VALIDATION LAYERS REQUESTED, BUT NONE ARE AVAILIBLE");

		//APP INFO
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Chaos Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Chaos";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		//CREATE INFO

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();


		if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: FAILED TO CREATE INSTANCE!");
		}
		else
		{
			LOGCORE_INFO("VULKAN: INSTANCE CREATED");
		}

		//PICKING PHYSICAL DEVICE//////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

		COREASSERT(deviceCount != 0, "VULKAN: FAILED TO FIND GPU WITH VULKAN SUPPORT");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				int score = RateDeviceSuitability(device);
				candidates.insert(std::make_pair(score, device));
			}
		}

		if (candidates.rbegin()->first > 0)
		{
			physicalDevice = candidates.rbegin()->second;
		}
		else
		{
			LOGCORE_ERROR("VULKAN: FAILED TO FIND SUITABLE GPU");
		}

		//PICKING LOGICAL DEVICE//////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
	}

	bool Renderer::checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const auto& layer : availableLayers)
		{
			LOGCORE_TRACE(layer.layerName);
		}

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}
			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	void Renderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		LOGCORE_WARN("VULKAN: VALIDATION LAYER: {0}", pCallbackData->pMessage);

		return VK_FALSE;
	}

	std::vector<const char*> Renderer::getRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		return extensions;
	}

	struct QueueFamilyIndices {

		std::optional<uint32_t> graphicsFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value();
		}
	};

	bool Renderer::IsDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}
			if (indices.isComplete())
			{
				break;
			}
			i++;
		}
		return indices.isComplete();
	}
	int Renderer::RateDeviceSuitability(VkPhysicalDevice device)
	{
		VkPhysicalDeviceFeatures deviceFeatures;
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		int score = 0;
		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}
		//Max possible texture size
		score += deviceProperties.limits.maxImageDimension2D;

		//Can't run without geometry shaders 
		if (!deviceFeatures.geometryShader)
		{
			return 0;
		}

		return score;
	}
}