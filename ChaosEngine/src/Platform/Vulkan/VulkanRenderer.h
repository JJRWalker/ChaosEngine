/* date = April 22nd 2021 8:39 am */

#ifndef _VULKAN_RENDERER_H
#define _VULKAN_RENDERER_H

#include "VulkanTypes.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/DataTypes/Vec4.h"

#include "VulkanMesh.h"
#include "VulkanMaterial.h"
#include "VulkanTexture.h"

#include <functional>
#include <deque>
#include <unordered_map>

#include "glm.hpp"
#include "GLM/glm/gtx/transform.hpp"

namespace Chaos
{
	class Window;
	class Camera;
	struct DeletionQueue
	{
		std::deque<std::function<void()>> Deletors;
		
		void push_function(std::function<void()>&& function)
		{
			Deletors.push_back(function);
		}
		
		void flush()
		{
			for (auto it = Deletors.rbegin(); it != Deletors.rend(); ++it)
			{
				(*it)();
			}
			Deletors.clear();
		}
	};
	
	class PipelineBuilder
	{
		public:
		VkPipeline BuildPipeline(VkDevice device, VkRenderPass pass);
		
		std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;
		VkPipelineVertexInputStateCreateInfo VertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo InputAssembly;
		VkViewport Viewport;
		VkRect2D Scissor;
		VkPipelineRasterizationStateCreateInfo Rasterizer;
		VkPipelineColorBlendAttachmentState ColourBlendAttachment;
		VkPipelineMultisampleStateCreateInfo Multisampling;
		VkPipelineLayout PipelineLayout;
		VkPipelineDepthStencilStateCreateInfo DepthStencil;
	};
	
	
	struct MeshPushConstants
	{
		glm::vec4 Data;
		glm::mat4 RenderMatrix;
	};
	
	struct GPUCameraData
	{
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProj;
	};
	
	
	struct GPUSceneData
	{
		Vec4 FogColour = { 0.2f, 0.2f, 0.2f, 0.1f };
		Vec4 FogDistances = { 0.1f,  0.5f, 1.0f, 2.0f};
		Vec4 AmbiantColour = { 0.5f, 0.5f, 0.5f, 0.2f };
		Vec4 SunlightDirection = { 1.0f, 1.0f, 0.0f, 1.0f };
		Vec4 SunlightColour = { 0.8f, 0.8f, 0.0f, 1.0f };
	};
	
	
	struct GPUObjectData
	{
		float ModelMatrix[16];
		float Optional[16];
	};
	
	
	struct FrameData
	{
		VkSemaphore PresentSemaphore, RenderSemaphore;
		VkFence RenderFence;
		
		VkCommandPool CommandPool;
		VkCommandBuffer MainCommandBuffer;
		
		AllocatedBuffer CameraBuffer;
		AllocatedBuffer ObjectBuffer;
		VkDescriptorSet ObjectDescriptor;
		
		VkDescriptorSet GlobalDescriptor;
	};
	
	
	struct VulkanUploadContext
	{
		VkFence UploadFence;
		VkCommandPool CommandPool;
	};
	
	
	constexpr unsigned int FRAME_OVERLAP = 2;
	
	class VulkanRenderer : public Renderer
	{
		friend class VulkanTexture;
		friend class VulkanMaterial;
		
		public:
		VulkanRenderer(Window* window);
		~VulkanRenderer();
		
		// Renderer interface
		RenderObject* AddQuad(float transform[16], Material* mat) override;
		void AddRenderable(RenderObject& toAdd) override;
		void RemoveRenderable(RenderObject* toRemove);
		
		void DrawLine(Vec2& startPoint, Vec2& endPoint, Vec4& colour, float weight, float renderOrder) {};
		
		//UI
		void DrawScreenSpaceQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<Texture> texture, float tilingFactor) {};
		void DrawScreenSpaceQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<SubTexture> subTexture) {};
		
		void DrawFrame();
		bool OnWindowResized(WindowResizeEvent& e) {  return false; };
		bool HasTexture(char* filePath, Ref<Texture> outTexture) { return false; };
		bool HasTexture(std::string name, Texture** outTexture = nullptr);
		bool HasTexture(std::string name, VulkanTexture** outTexture = nullptr);
		
		void InitImgui() override;
		void SetCamera(Camera* camera) override;
		
		void UploadMesh(VulkanMesh& mesh);
		void UploadMaterial(VulkanMaterial& mat);
		void UploadTexture(VulkanTexture& tex);
		
		void DrawObjects(VkCommandBuffer cmd, RenderObject* first, size_t count);
		
		void Init();
		void Cleanup();
		
		AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		
		VulkanTexture* CreateTexture(std::string filepath);
		// use to get a pointer to the blank texture, do not use to initialise a new tex to blank
		VulkanTexture* GetBlankTexture();
		VulkanTexture* CreateBlankTexture(std::string name);
		
		VulkanMaterial* CreateMaterial(std::string name, Texture* texture, std::string fragShaderPath, std::string vertShaderPath);
		void InitDefaultMaterials();
		
		private:
		void InitVulkan();
		void InitSwapchain();
		void InitDefaultRenderPass();
		void InitFrameBuffers();
		void InitCommands();
		void InitSyncStructures();
		void InitDescriptors();
		void InitDefaultPipeline();
		void InitSamplers();
		
		void CleanupSwapchain();
		void RecreateSwapchain();
		
		bool LoadShaderModule(const char* filePath, VkShaderModule* outShaderModule);
		
		void LoadDefaultMeshes();
		
		void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);
		
		VulkanMaterial* GetMaterial(const std::string& name);
		VulkanMesh* GetMesh(const std::string& name);
		
		FrameData& GetCurrentFrame();
		
		size_t PadUniformBufferSize(size_t originalSize);
		
		public:
		bool IsInitialized = false;
		int FrameNumber = 0;
		
		Camera* pCamera;
		Window* pWindow;
		VkExtent2D WindowExtent;
		DeletionQueue MainDeletionQueue;
		DeletionQueue SwapchainDeletionQueue;
		
		std::vector<RenderObject> Renderables;
		std::unordered_map<std::string, VulkanMaterial> Materials;
		std::unordered_map<std::string, VulkanMesh> Meshes;
		std::unordered_map<std::string, VulkanTexture> Textures;
		
		VulkanUploadContext UploadContext;
		
		FrameData Frames[FRAME_OVERLAP];
		
		private:
		VmaAllocator m_allocator;
		
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;
		VkPhysicalDevice m_physicalDevice;
		VkDevice m_device;
		
		VkPhysicalDeviceProperties m_physicalDeviceProperties;
		
		VkQueue m_graphicsQueue;
		uint32_t m_graphicsQueueFamily;
		
		VkCommandPool m_commandPool;
		VkCommandBuffer m_mainCommandBuffer;
		
		VkRenderPass m_renderPass;
		
		VkSurfaceKHR m_surface;
		VkSwapchainKHR m_swapchain;
		VkFormat m_swapchainImageFormat;
		
		std::vector<VkFramebuffer> m_framebuffers;
		std::vector<VkImage> m_swapchainImages;
		std::vector<VkImageView> m_swapchainImageViews;
		
		VkImageView m_depthImageView;
		AllocatedImage m_depthImage;
		
		VkFormat m_depthFormat;
		
		VkDescriptorSetLayout m_globalSetLayout;
		VkDescriptorSetLayout m_objectSetLayout;
		VkDescriptorSetLayout m_singleTextureSetLayout;
		VkDescriptorPool m_descriptorPool;
		
		GPUSceneData m_sceneParameters;
		AllocatedBuffer m_sceneParameterBuffer;
		
		VkSampler m_nearestNeighbourSampler;
		
		const Vec3 QUAD_VERTEX_POSITIONS[6] = 
		{
			{-0.5f, -0.5f, 0.0f},
			{0.5f, -0.5f, 0.0f},
			{0.5f, 0.5f, 0.0f},
			{0.5f, 0.5f, 0.0f},
			{-0.5f, 0.5f, 0.0f},
			{-0.5f, -0.5f, 0.0f}
		};
	};
}

#endif //_VULKAN_RENDERER_H
