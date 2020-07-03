#pragma once
#include "Chaos/Renderer/Renderer.h"

#include <Vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#include <GLM/glm/glm.hpp>

#include <optional>
#include <array>

#include "Chaos/DataTypes/Vec3.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec4.h"
#include "Platform/Vulkan/VulkanTexture.h"
#include "Chaos/Renderer/PrimitiveType.h"

namespace Chaos
{
	struct VulkanVertex {
		Vec2 pos;
		Vec4 color;
		Vec2 texCoord;
		float texIndex;

		static VkVertexInputBindingDescription GetBindingDescriptions() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(VulkanVertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(VulkanVertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(VulkanVertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(VulkanVertex, texCoord);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(VulkanVertex, texIndex);

			return attributeDescriptions;
		}
	};

	enum class BufferType {	Static, Dynamic};

	struct Buffer
	{
		VkBuffer VertexBuffer;
		VkDeviceMemory VertexBufferMemory;
		VkBuffer IndexBuffer;
		VkDeviceMemory IndexBufferMemory;
		uint64_t VertexCount = 0;
		uint64_t IndexCount = 0;
		uint16_t HighestInd = 0;
		BufferType Type;
		std::vector<Ref<VulkanTexture>> TexturesToBind;
		size_t DescriptorIndex = 0;
		size_t TexturesLoaded = 0;
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

		virtual void DrawQuad(Vec2& position, Vec2& scale, Ref<Texture> texture) override;
		virtual void DrawQuad(Vec2& position, Vec2& scale, Vec4& colour, Ref<Texture> texture) override;
		virtual void DrawQuad(Vec2& position, Vec2& scale, Vec4& colour, Ref<Texture> texture, float tilingFactor) override;
		virtual void DrawQuad(Vec2& position, Vec2& scale, Ref<Texture> texture, float tilingFactor) override;
		virtual void DrawQuad(Vec2& position, Vec2& scale, Ref<SubTexture> subTexture) override;
		virtual void DrawFrame() override;
		virtual void WindowResized() override { mFramebufferResized = true; }
		bool HasTexture(const char* filePath, Ref<Texture> outTexture) override; //Takes in a file path and a texture, returns true and sets the ref of inputted texture if one exists
		//VULKAN TEMP
	private:
		//constants
		const glm::vec4 CLEAR_COLOR = { 0.0f,0.0f, 0.03f, 1.0f };
		const int MAX_FRAMES_IN_FLIGHT = 2;
		const int MAX_OBJECTS_PER_DRAW = 10000;
		const int MAX_TEXTURES_PER_DRAW = 31;

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
		void CreateTextureSampler();
		void CreateIndexedBuffer(std::vector<VulkanVertex> vertices, std::vector<uint16_t> indices, BufferType type, size_t insertIndex);
		void CreateAndClearBuffers(size_t insertIndex);
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSet();
		void CreateCommandBuffers();
		void CreateSyncObjects();

		void CleanUpSwapchain();
		void RecreateSwapchain();

		void UpdateUniformBuffers();

		//Helper functions
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		//VALIDATION
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		//SHADER LOADING
		static std::vector<char> readFile(const std::string& filename);
		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		//IMAGE
		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkImageView CreateImageView(VkImage image, VkFormat format);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		//BUFFER 
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		//COMMANDS
		VkCommandBuffer  BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		//IMGUI
		void SetImGuiCommandBuffer(std::vector<VkCommandBuffer> mCommandBuffers) { mImGuiCommandBuffers = mCommandBuffers; }
		void SetImGuiCommandPool(VkCommandPool* pool) { mImGuiCommandPool = pool; }
		void SetImGuiFramebuffer(std::vector<VkFramebuffer>* buffer) { mImGuiFrameBuffer = buffer; }

		//Variables
		std::vector<VulkanVertex> mVertices;
		std::vector<uint16_t> mIndices;

		std::vector<Ref<VulkanTexture>> mTexturesToBind;
		int mRenderCount = 0;
		uint16_t mIndOffset = 0;

		VkSampler mTextureSampler;

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

		std::vector<Buffer> mBuffers;

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

		//for viewport
		std::vector<VkImage> mRenderedFrames;
		std::vector<VkDeviceMemory> mRenderedFramesMemory;
		std::vector<VkImageView> mRenderedFrameViews;
		std::vector<VkFramebuffer> mRenderedFrameBuffers;
		VkRenderPass mRenderedRenderPass;

		const std::vector<const char*> mValidationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> mDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		//DEBUG VARS
		int mTotalQuadsDrawn;
	};
}