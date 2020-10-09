#pragma once
#include "Chaos/Renderer/Renderer.h"
#include <Vulkan/Include/vulkan/vulkan.h>

//Ideally I'd like to avoid including GLM here but I don't have a mat4 data type that has the functionality of GLM's
#define GLM_FORCE_RADIANS
#include <GLM/glm/glm.hpp>
#include <array>

#include "Chaos/DataTypes/Vec3.h"

namespace Chaos
{
	class VulkanTexture;
	class Vec2;
	class Vec4;
	
	struct Quad
	{
		Vec3 Position = {0,0,0};
		Vec2 Scale = {1,1};
		Vec2 Rotation = {0, 0};
		Vec4 Colour = {1,1,1,1};
		Ref<Texture> tex;
		Ref<SubTexture> subTex;
		float tilingFactor = 1;
		
		//we sort quads based on their z position. This z position works as a render queue position
		bool operator < (const Quad& other) const
		{
			return Position.Z < other.Position.Z;
		}
	};
	
	struct VulkanVertex {
		Vec3 pos;
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
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
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
		
		virtual void DrawQuad(Vec3& position, Vec2& scale, Ref<Texture> texture) override;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec4& colour, Ref<Texture> texture) override;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec4& colour, Ref<Texture> texture, float tilingFactor) override;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Ref<Texture> texture, float tilingFactor) override;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Ref<SubTexture> subTexture) override;
		//with rotation
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec2& rotation, Ref<Texture> texture) override;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<Texture> texture) override;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<Texture> texture, float tilingFactor) override;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec2& rotation, Ref<SubTexture> subTexture) override;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<SubTexture> subTtexture) override;
		
		virtual void DrawLine(Vec2& startPoint, Vec2& endPoint, Vec4& colour, float weight, float renderOrder) override;
		
		//ui
		virtual void DrawScreenSpaceQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<Texture> texture, float tilingFactor) override;
		virtual void DrawScreenSpaceQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<SubTexture> subTexture) override; 
		
		virtual void DrawFrame() override;
		virtual void WindowResized() override { m_framebufferResized = true; }
		virtual bool HasTexture(char* filePath, Ref<Texture> outTexture) override;
		virtual bool HasTexture(std::string filePath, Ref<Texture> outTexture) override;//Takes in a file path and a texture, returns true and sets the ref of inputted texture if one exists
		virtual RenderData& GetDebugInfo() override { return m_debugInfo; }
		
		VkSampler& GetTexSampler() { return m_textureSampler; }
		std::vector<VkImageView>& GetRenderedFrames() { return m_renderedFrameViews; }
		size_t GetCurrentFrame() { return m_currentFrame; }
		
		//VULKAN TEMP
		private:
		//constants
		const Vec4 CLEAR_COLOR = { 0.0f,0.0f, 0.03f, 1.0f };
		const int MAX_FRAMES_IN_FLIGHT = 2;
		const int MAX_OBJECTS_PER_DRAW = 10000;
		const int MAX_TEXTURES_PER_DRAW = 31;
		
		//Funcs
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT mDebugMessenger, const VkAllocationCallbacks* pAllocator);
		
		void AddQuadToRenderQueue(Quad quad);
		
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
		void CreateBuffersAndClearResources(size_t insertIndex);
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
		void SortQuads();
		
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
		void SetImGuiCommandBuffer(std::vector<VkCommandBuffer> mCommandBuffers) { m_imGuiCommandBuffers = mCommandBuffers; }
		void SetImGuiCommandPool(VkCommandPool* pool) { m_imGuiCommandPool = pool; }
		void SetImGuiFramebuffer(std::vector<VkFramebuffer>* buffer) { m_imGuiFrameBuffer = buffer; }
		
		//Variables
		std::vector<Quad> m_quads;
		std::vector<VulkanVertex> m_vertices;
		std::vector<uint16_t> m_indices;
		
		std::vector<Ref<VulkanTexture>> m_texturesToBind;
		int m_renderCount = 0;
		uint16_t m_indOffset = 0;
		
		VkSampler m_textureSampler;
		
		bool m_framebufferResized = false;
		bool m_renderingGUI = false;
		size_t m_currentFrame = 0;
		
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;
		VkSurfaceKHR m_surface;
		
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkDevice m_device;
		
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;
		
		VkRenderPass m_renderpass;
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkPipelineLayout m_pipelineLayout;
		
		VkPipeline m_graphicsPipeline;
		
		std::mutex m_commandPoolMutex;
		VkCommandPool m_commandPool;
		VkCommandPool* m_imGuiCommandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::vector<VkCommandBuffer> m_imGuiCommandBuffers;
		
		std::vector<VkFramebuffer>* m_imGuiFrameBuffer;
		
		std::vector<Buffer> m_buffers;
		
		VkDescriptorPool m_descriptorPool;
		std::vector<VkDescriptorSet> m_descriptorSets;
		
		std::vector<VkBuffer> m_uniformBuffers;
		std::vector<VkDeviceMemory> m_uniformBuffersMemory;
		
		bool m_waitingOnFences = true;
		std::vector<VkFence> m_imagesInFlight;
		std::vector<VkFence> m_inFlightFences;
		std::vector<VkSemaphore>m_imageAvailableSemaphores;
		std::vector<VkSemaphore>m_renderFinishedSemaphores;
		
		VkSwapchainKHR m_swapchain;
		std::vector<VkImageView> m_swapchainImageViews;
		std::vector<VkFramebuffer> m_swapchainframebuffers;
		std::vector<VkImage> m_swapchainImages;
		uint32_t m_imageIndex = 0;
		VkFormat m_swapchainImageFormat;
		VkExtent2D m_swapchainExtent;
		
		//for viewport
		std::vector<VkImage> m_renderedFrames;
		std::vector<VkDeviceMemory> m_renderedFramesMemory;
		std::vector<VkImageView> m_renderedFrameViews;
		std::vector<VkFramebuffer> m_renderedFrameBuffers;
		VkRenderPass m_renderedRenderPass;
		
		const std::vector<const char*> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> m_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		
		const glm::vec4 QUAD_VERTEX_POSITIONS[4] = { glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f) ,
			glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
			glm::vec4(0.5f, 0.5f, 0.0f, 1.0f),
			glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f) };
		
		const uint16_t QUAD_INDICES[6] = { 0,1,2,2,3,0 };
		
		//DEBUG VARS
		RenderData m_debugInfo;
	};
}