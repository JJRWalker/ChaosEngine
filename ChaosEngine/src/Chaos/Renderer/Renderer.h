
#pragma once
//#define GLFW_INCLUDE_VULKAN
#include <Vulkan/Include/vulkan/vulkan.h>
//#include <GLFW/glfw3.h>
//#include <GLM/glm/glm.hpp>

#include <optional>

namespace Chaos
{
	enum class RendererAPI
	{
		None = 0, Vulkan = 1
	};

	struct QueueFamilyIndices {

		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	class Renderer
	{


	public:
		Renderer();
		~Renderer();

		inline static RendererAPI GetAPI() { return sRendererAPI; }

		static Renderer* Create() { return new Renderer(); }

		void DrawFrame();
		bool WaitIdle();
	private:
		static RendererAPI sRendererAPI;


		//VULKAN TEMP
	private:

		//Funcs
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

		void InitVulkan();
		void CreateInstance();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateGraphicsPipeline();
		void CreateFrameBuffers();
		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateSemaphores();
		
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		static std::vector<char> readFile(const std::string& filename);
		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		VkInstance vkInstance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkSurfaceKHR vkSurface;

		VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
		VkDevice vkDevice;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkRenderPass vkRenderPass;
		VkPipelineLayout vkPipelineLayout;

		VkPipeline vkGraphicsPipeline;

		VkCommandPool vkCommandPool;
		std::vector<VkCommandBuffer> commandBuffers;

		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;

		VkSwapchainKHR vkSwapchain;
		std::vector<VkImageView> swapchainImageViews;
		std::vector<VkFramebuffer> swapchainframebuffers;
		std::vector<VkImage> swapchainImages;
		VkFormat swapchainImageFormat;
		VkExtent2D swapchainExtent;

		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };		
	};
}