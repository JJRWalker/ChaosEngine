/* date = April 22nd 2021 8:39 am */

#ifndef _VULKAN_RENDERER_H
#define _VULKAN_RENDERER_H

#include "VulkanTypes.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/DataTypes/ChaoticArray.h"
#include "Chaos/DataTypes/Array.h"

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
	
	
	struct GPUCameraData
	{
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProj;
	};
	
	
	struct FrameData
	{
		VkSemaphore PresentSemaphore, RenderSemaphore;
		VkFence RenderFence;
		
		VkCommandPool CommandPool;
		VkCommandBuffer MainCommandBuffer;
		
		AllocatedBuffer CameraBuffer;
		AllocatedBuffer LightingBuffer;
		VkDescriptorSet LightingDescriptor;
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
	const unsigned int MAX_OBJECTS = 100000;
	const unsigned int MAX_LIGHTS = 100;
	const size_t MAX_DESCRIPTOR_SETS = 1000;
	
	class VulkanRenderer : public Renderer
	{
		friend class VulkanTexture;
		friend class VulkanMaterial;
		friend class ImGuiEditor;
		
		public:
		VulkanRenderer(Window* window);
		~VulkanRenderer();
		
		// Renderer interface
		RenderObject* AddQuad(float transform[16], Material* mat) override;
		RenderObject* AddRenderable(RenderObject* toAdd) override;
		void RemoveRenderable(RenderObject* toRemove) override;
		
		LightingObjectData* AddLight(float transform[16]) override;
		void RemoveLight(LightingObjectData* light) override;
		
		void DrawLine(Vec2& startPoint, Vec2& endPoint, Vec4& colour, float weight, float renderOrder) override;
		
		void DrawFrame();
		bool OnWindowResized(WindowResizeEvent& e) {  return false; };
		bool HasTexture(char* filePath, Ref<Texture> outTexture) { return false; };
		bool HasTexture(std::string name, Texture** outTexture = nullptr);
		bool HasTexture(std::string name, VulkanTexture** outTexture = nullptr);
		
		void InitImgui() override;
		
		void SetVSync(bool state) override;
		bool GetVSync() override;
		
		void* GetImguiEditorPanelTextureID() override;
		
		void UploadMesh(VulkanMesh& mesh);
		void UploadMaterial(VulkanMaterial& mat);
		void UploadTexture(VulkanTexture& tex);
		
		void DrawObjects(VkCommandBuffer cmd, ChaoticArray<RenderObject*>& renderObjData, ChaoticArray<LightingObjectData*>& lightingData);
		
		void Init() override;
		void Cleanup();
		
		AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		
		VulkanTexture* CreateTexture(std::string filepath);
		// use to get a pointer to the blank texture, do not use to initialise a new tex to blank
		VulkanTexture* GetBlankTexture();
		VulkanTexture* CreateBlankTexture(std::string name);
		
		VulkanMaterial* CreateMaterial(std::string name, std::string fragShaderPath, std::string vertShaderPath);
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
		
		VulkanMaterial& RecreateMaterial(VulkanMaterial& mat);
		void RecreateMaterials();
		
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
		
		Window* pWindow;
		VkExtent2D WindowExtent;
		DeletionQueue MainDeletionQueue;
		DeletionQueue SwapchainDeletionQueue;
		
		//TODO OPTIMISATION: Multi thread recording of renderables. Currently has a big overhead here. Divide and conquer!
		//NOTE: Should be stable frame rate when inserting and removing, but has to itterate over it's max capacity every time. Lights however, need to be in a standard array as we need to push the number of lights in the scene to the shader.
		ChaoticArray<RenderObject*> Renderables = ChaoticArray<RenderObject*>(MAX_OBJECTS);
		ChaoticArray<LightingObjectData*> Lights = ChaoticArray<LightingObjectData*>(MAX_LIGHTS);
		std::unordered_map<std::string, VulkanMaterial> Materials;
		std::unordered_map<std::string, VulkanMesh> Meshes;
		std::unordered_map<std::string, VulkanTexture> Textures;
		
		VulkanUploadContext UploadContext;
		
		FrameData Frames[FRAME_OVERLAP];
		
		private:
		bool m_vsync = false;
		VkPresentModeKHR m_presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		
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
		VkDescriptorSetLayout m_lightingSetLayout;
		VkDescriptorSetLayout m_singleTextureSetLayout;
		VkDescriptorPool m_descriptorPool;
		VkFence m_descriptorFence;
		
		AllocatedBuffer m_levelParametersBuffer;
		
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
		
		const Vec2 QUAD_UV_POSITIONS[6] = 
		{
			{0.0f, 1.0f},
			{1.0f, 1.0f},
			{1.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 1.0f},
		};
	};
}

#endif //_VULKAN_RENDERER_H
