#pragma once
#include "Chaos/Renderer/Renderer.h"

#include <Vulkan/Include/vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#include <GLM/glm/glm.hpp>

#include <optional>
#include <array>

#include "Chaos/DataTypes/Vec3.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Platform/Vulkan/VulkanTexture.h"
#include "Chaos/Renderer/PrimitiveType.h"

namespace Chaos
{
	struct VulkanVertex {
		Vec2 pos;
		Vec3 color;
		Vec2 texCoord;

		static VkVertexInputBindingDescription GetBindingDescriptions() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(VulkanVertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(VulkanVertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(VulkanVertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(VulkanVertex, texCoord);

			return attributeDescriptions;
		}
	};

	struct RenderData {
		std::vector<VulkanVertex> Vertices;
		std::vector<uint16_t> Indices;
		Texture* Texture;
	};

	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
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

	class VulkanRenderer : public Renderer
	{
		friend class ImGuiLayer;
		friend class VulkanTexture;

	public:
		VulkanRenderer();
		~VulkanRenderer();

		virtual void DrawQuad(Vec2& position, Vec2& scale, Texture* texture) override;
		virtual void DrawFrame() override;
		virtual bool WaitIdle() override;
		virtual void WindowResized() override { mFramebufferResized = true; }
		//VULKAN TEMP
	private:

		std::vector<RenderData> mRenderQueue;

		const glm::vec4 mClearColor = { 0.0f,0.0f, 0.03f, 1.0f };

		VkSampler textureSampler;

		//Funcs
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT mDebugMessenger, const VkAllocationCallbacks* pAllocator);

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
		void CreateDescriptorSetLayout();
		void CreateGraphicsPipeline();
		void CreateFrameBuffers();
		void CreateCommandPool();
		void CreateTextureImage(Texture* tex);
		void CreateTextureSampler();
		void CreateVertexBuffers(std::vector<VulkanVertex> vertices, size_t insertIndex);
		void CreateIndexBuffers(std::vector<uint16_t> indices, size_t insertIndex);
		void PopulateBuffers(size_t renderQueueIndex);
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void CreateCommandBuffers();
		void CreateSyncObjects();

		void CleanUpSwapchain();
		void RecreateSwapchain();

		void UpdateUniformBuffers();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		static std::vector<char> readFile(const std::string& filename);

		VkShaderModule CreateShaderModule(const std::vector<char>& code);
		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkImageView CreateImageView(VkImage image, VkFormat format);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		VkCommandBuffer  BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		void SetImGuiCommandBuffer(std::vector<VkCommandBuffer> mCommandBuffers) { mImGuiCommandBuffers = mCommandBuffers; }
		void SetImGuiCommandPool(VkCommandPool* pool) { mImGuiCommandPool = pool; }
		void SetImGuiFramebuffer(std::vector<VkFramebuffer>* buffer) { mImGuiFrameBuffer = buffer; }

		//Vars
		std::vector<std::vector<VulkanVertex>> mVertices;
		std::vector<std::vector<uint16_t>> mIndicies;

		const int MAX_FRAMES_IN_FLIGHT = 2;
		bool mFramebufferResized = false;
		bool mRenderingGUI = false;
		size_t mCurrentFrame = 0;

		VkInstance mInstance;
		VkDebugUtilsMessengerEXT mDebugMessenger;
		VkSurfaceKHR mSurface;

		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice;

		VkQueue mGraphicsQueue;
		VkQueue mPresentQueue;

		VkRenderPass mRenderPass;
		VkDescriptorSetLayout mDescriptorSetLayout;
		VkPipelineLayout mPipelineLayout;

		VkPipeline mGraphicsPipeline;

		std::mutex commandPoolMutex;
		VkCommandPool mCommandPool;
		VkCommandPool* mImGuiCommandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> mCommandBuffers;
		std::vector<VkCommandBuffer> mImGuiCommandBuffers;

		std::vector<VkFramebuffer>* mImGuiFrameBuffer;

		std::vector<VkBuffer> mVertexBuffers;
		std::vector<VkDeviceMemory> mVertexBuffersMemory;
		std::vector<VkBuffer> mIndexBuffers;
		std::vector<VkDeviceMemory> mIndexBuffersMemory;

		VkDescriptorPool mDescriptorPool;
		std::vector<VkDescriptorSet> mDescriptorSets;

		std::vector<VkBuffer> mUniformBuffers;
		std::vector<VkDeviceMemory> mUniformBuffersMemory;

		bool mWaitingOnFences = true;
		std::vector<VkFence> mImagesInFlight;
		std::vector<VkFence> mInFlightFences;
		std::vector<VkSemaphore>mImageAvailableSemaphores;
		std::vector<VkSemaphore>mRenderFinishedSemaphores;

		VkSwapchainKHR mSwapchain;
		std::vector<VkImageView> mSwapchainImageViews;
		std::vector<VkFramebuffer> mSwapchainframebuffers;
		std::vector<VkImage> mSwapchainImages;
		uint32_t mImageIndex = 0;
		VkFormat mSwapchainImageFormat;
		VkExtent2D mSwapchainExtent;

		const std::vector<const char*> mValidationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> mDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};
}