
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <Vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <GLM/glm/glm.hpp>

namespace Chaos
{
	enum class RendererAPI
	{
		None = 0, Vulkan = 1
	};

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		inline static RendererAPI GetAPI() { return sRendererAPI; }

	private:
		static RendererAPI sRendererAPI;


		//VULKAN TEMP
	private:
		void VulkanInit();
		bool checkValidationLayerSupport();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
		std::vector<const char*> getRequiredExtensions();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		int RateDeviceSuitability(VkPhysicalDevice device);


		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation"};

		VkInstance vkInstance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice; //implicitly destroyed when instance is destroyed
	};
}