
#pragma once
//#define GLFW_INCLUDE_VULKAN
#include <Vulkan/Include/vulkan/vulkan.h>
//#include <GLFW/glfw3.h>
#include <GLM/glm/glm.hpp>

#include <optional>
#include <array>

namespace Chaos
{
	enum class RendererAPI
	{
		None = 0, Vulkan = 1
	};

	struct Vertex
	{
		glm::vec2 Pos;
		glm::vec3 Color;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription GetBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, Pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, Color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}
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
		void WindowResized() { framebufferResized = true; }
	private:
		static RendererAPI sRendererAPI;


		//VULKAN TEMP
	private:

		const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f},{0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f},	{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}

		};

		const glm::vec4 mClearColor = { 0.0f,0.0f, 0.03f, 1.0f };

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;

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
		void CreateTextureImage();
		void CreateTextureImageView();
		void CreateTextureSampler();
		void CreateVertexBuffers();
		void CreateCommandBuffers();
		void CreateSyncObjects();

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		VkCommandBuffer  BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		VkImageView CreateImageView(VkImage image, VkFormat format);

		void CleanUpSwapchain();
		void RecreateSwapchain();
		
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
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);

		//Vars
		const int MAX_FRAMES_IN_FLIGHT = 2;
		bool framebufferResized = false;
		size_t currentFrame = 0;

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

		VkBuffer vkVertexBuffer;
		VkDeviceMemory vertexBufferMemory;

		std::vector<VkFence> imagesInFlight;
		std::vector<VkFence> inFlightFences;
		std::vector<VkSemaphore>imageAvailableSemaphores;
		std::vector<VkSemaphore>renderFinishedSemaphores;

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