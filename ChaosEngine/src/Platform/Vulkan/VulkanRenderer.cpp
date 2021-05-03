#include "chaospch.h"
#include "VulkanRenderer.h"
#include "VulkanInitalizers.h"

#include "VKBootstrap/VkBootstrap.h"
#include "VKBootstrap/VkBootstrap.cpp"

#include "Chaos/Nodes/Camera.h"
#include "Chaos/Core/Window.h"
#include "Chaos/Serialisation/FileUtils.h"

#include "GLFW/glfw3.h"
#include "ImGUI/imgui.h"
#include "ImGUI/examples/imgui_impl_vulkan.h"
#include "ImGUI/examples/imgui_impl_glfw.h"

#define VMA_IMPLEMENTATION
#include "Vma/vk_mem_alloc.h"

#include <iostream>
#include <fstream>

#ifdef CHAOS_DEBUG
const bool USE_VALIDATION_LAYERS = true;

#define VK_CHECK(x)												\
do													\
{													\
VkResult err = x;								\
if (err)										\
{												\
LOGCORE_ERROR("VULKAN: ERROR: {0}", err);	\
}												\
}													\
while (0)
#else
const bool USE_VALIDATION_LAYERS = false;
#define VK_CHECK(x)	x
#endif

const int MAX_OBJECTS = 100000;

namespace Chaos
{	VulkanRenderer::VulkanRenderer(Window* window) : pWindow(window)
	{
		Init();
	}
	
	VulkanRenderer::~VulkanRenderer()
	{
		Cleanup();
	}
	
	
	RenderObject* VulkanRenderer::AddQuad(float transform[16], Material* mat)
	{
		RenderObject quad;
		quad.Mesh = GetMesh("quad");
		quad.Material = mat;
		quad.RenderID = Renderables.size();
		memcpy((void*)&quad.Transform[0], (void*)&transform[0], sizeof(float) * 16);
		
		Renderables.push_back(quad);
		
		return &Renderables[Renderables.size() - 1];
	}
	
	
	void VulkanRenderer::AddRenderable(RenderObject& toAdd)
	{
		toAdd.RenderID = Renderables.size();
		Renderables.push_back(toAdd);
	}
	
	
	void VulkanRenderer::RemoveRenderable(RenderObject* toRemove)
	{
		Renderables.erase(Renderables.begin() + toRemove->RenderID);
	}
	
	
	bool VulkanRenderer::HasTexture(std::string name, Texture** outTexture)
	{
		if (Textures.find(name) != Textures.end())
		{
			*outTexture = &Textures[name];
			return true;
		}
		return false;
	}
	
	
	bool VulkanRenderer::HasTexture(std::string name, VulkanTexture** outTexture)
	{
		if (Textures.find(name) != Textures.end())
		{
			*outTexture = &Textures[name];
			return true;
		}
		return false;
	}
	
	
	void VulkanRenderer::DrawFrame()
	{
		VK_CHECK(vkWaitForFences(m_device, 1, &GetCurrentFrame().RenderFence, true, 1000000000));
		VK_CHECK(vkResetFences(m_device, 1, &GetCurrentFrame().RenderFence));
		
		VK_CHECK(vkResetCommandBuffer(GetCurrentFrame().MainCommandBuffer, 0));
		
		uint32_t swapChainImageIndex;
		VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, GetCurrentFrame().PresentSemaphore, nullptr, &swapChainImageIndex);
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			LOGCORE_ERROR("VULKAN: failed to aquire swapchain image!");
		}
		
		VkCommandBuffer cmd = GetCurrentFrame().MainCommandBuffer;
		
		VkCommandBufferBeginInfo cmdBeginInfo = VkInit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		
		VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
		
		VkClearValue clearValue;
		clearValue.color = { { 0.0f, 0.0f, 0.1f, 1.0f } };
		
		VkClearValue depthClear;
		depthClear.depthStencil.depth = 1.0f;
		
		VkRenderPassBeginInfo rpInfo = VkInit::RenderpassBeginInfo(m_renderPass, WindowExtent, m_framebuffers[swapChainImageIndex]);
		
		rpInfo.clearValueCount = 2;
		
		VkClearValue clearValues[] = { clearValue, depthClear };
		
		rpInfo.pClearValues = &clearValues[0];
		
		vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		DrawObjects(cmd, Renderables.data(), Renderables.size());
		
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
		
		vkCmdEndRenderPass(cmd);
		
		VK_CHECK(vkEndCommandBuffer(cmd));
		
		VkSubmitInfo submit = VkInit::SubmitInfo(&cmd);
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		
		submit.pWaitDstStageMask = &waitStage;
		
		submit.waitSemaphoreCount = 1;
		submit.pWaitSemaphores = &GetCurrentFrame().PresentSemaphore;
		
		submit.signalSemaphoreCount = 1;
		submit.pSignalSemaphores = &GetCurrentFrame().RenderSemaphore;
		
		VK_CHECK(vkQueueSubmit(m_graphicsQueue, 1, &submit, GetCurrentFrame().RenderFence));
		
		VkPresentInfoKHR presentInfo = VkInit::PresentInfo();
		
		presentInfo.pSwapchains = &m_swapchain;
		presentInfo.swapchainCount = 1;
		
		presentInfo.pWaitSemaphores = &GetCurrentFrame().RenderSemaphore;
		presentInfo.waitSemaphoreCount = 1;
		
		presentInfo.pImageIndices = &swapChainImageIndex;
		
		VK_CHECK(vkQueuePresentKHR(m_graphicsQueue, &presentInfo));
		
		FrameNumber++;
	}
	
	
	void VulkanRenderer::DrawObjects(VkCommandBuffer cmd, RenderObject* first, size_t count)
	{
		GPUCameraData camData;
		if (pCamera)
		{
			camData.projection = pCamera->GetProjection();
			camData.view = pCamera->GetView();
			camData.viewProj = camData.projection * camData.view;
		}
		
		void* data;
		vmaMapMemory(m_allocator, GetCurrentFrame().CameraBuffer.Allocation, &data);
		
		memcpy(data, &camData, sizeof(GPUCameraData));
		
		vmaUnmapMemory(m_allocator, GetCurrentFrame().CameraBuffer.Allocation);
		
		float framed = (FrameNumber / 120.f);
		
		//m_sceneParameters.AmbiantColour = {sin(framed), 0, cos(framed), 1};
		m_sceneParameters.SunlightDirection.X = -sin(framed);
		
		char* sceneData;
		vmaMapMemory(m_allocator, m_sceneParameterBuffer.Allocation, (void**)&sceneData);
		
		int frameIndex = FrameNumber % FRAME_OVERLAP;
		
		sceneData += PadUniformBufferSize(sizeof(GPUSceneData) * frameIndex);
		
		memcpy(sceneData, &m_sceneParameters, sizeof(GPUSceneData));
		
		vmaUnmapMemory(m_allocator, m_sceneParameterBuffer.Allocation);
		
		
		void* objectData;
		vmaMapMemory(m_allocator, GetCurrentFrame().ObjectBuffer.Allocation, &objectData);
		
		GPUObjectData* sobjectSSBO = (GPUObjectData*)objectData;
		
		for (int i = 0; i < count; ++i)
		{
			RenderObject& obj = first[i];
			memcpy((void*)&sobjectSSBO[i].ModelMatrix, (void*)&obj.Transform, sizeof(float) * 16);
			sobjectSSBO[i].Optional[0] = 23.4f;
		}
		
		vmaUnmapMemory(m_allocator, GetCurrentFrame().ObjectBuffer.Allocation);
		
		Mesh* lastMesh = nullptr;
		Material* lastMaterial = nullptr;
		
		for (int i = 0; i < count; ++i)
		{
			RenderObject& obj = first[i];
			
			if (obj.Material != lastMaterial)
			{
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ((VulkanMaterial*)obj.Material)->Pipeline);
				lastMaterial = obj.Material;
				
				uint32_t uniformOffset = (uint32_t)PadUniformBufferSize(sizeof(GPUSceneData) * frameIndex);
				
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ((VulkanMaterial*)obj.Material)->PipelineLayout, 0, 1, &GetCurrentFrame().GlobalDescriptor, 1, &uniformOffset);
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ((VulkanMaterial*)obj.Material)->PipelineLayout, 1, 1, &GetCurrentFrame().ObjectDescriptor, 0, nullptr);
				if (((VulkanMaterial*)obj.Material)->TextureSet != VK_NULL_HANDLE)
				{
					vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ((VulkanMaterial*)obj.Material)->PipelineLayout, 2, 1, &((VulkanMaterial*)obj.Material)->TextureSet, 0, nullptr);
				}
			}
			
			//glm::mat4 model = obj.Transform;
			//final render matrix, that we are calculating on the cpu
			//glm::mat4 meshMatrix = model;
			
			
			MeshPushConstants constants;
			//constants.RenderMatrix = meshMatrix;
			
			vkCmdPushConstants(cmd, ((VulkanMaterial*)obj.Material)->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);
			
			if (obj.Mesh != lastMesh)
			{
				VkDeviceSize offset = 0;
				vkCmdBindVertexBuffers(cmd, 0, 1, &((VulkanMesh*)obj.Mesh)->VertexBuffer.Buffer, &offset);
				lastMesh = obj.Mesh;
			}
			
			vkCmdDraw(cmd, (uint32_t)obj.Mesh->Vertices.size(), 1, 0, i);
		}
	}
	
	
	void VulkanRenderer::SetCamera(Camera* camera)
	{
		pCamera = camera;
	}
	
	
	VulkanTexture* VulkanRenderer::CreateTexture(std::string filepath)
	{
		VulkanTexture* outTex = nullptr;
		if (HasTexture(FileUtils::GetFileName(filepath, false), &outTex))
			return outTex;
		
		VulkanTexture texture = VulkanTexture(this, filepath);
		UploadTexture(texture);
		outTex = &Textures[texture.Name];
		return outTex;
	}
	
	
	VulkanTexture* VulkanRenderer::GetBlankTexture()
	{
		VulkanTexture* outTex = nullptr;
		if (HasTexture("blank", &outTex))
			return outTex;
		
		VulkanTexture blank = VulkanTexture(this);
		blank.LoadProtection = true; // stops it from being loaded into, we don't want blank to be changed
		UploadTexture(blank);
		outTex = &Textures["blank"];
		return outTex;
	}
	
	
	VulkanTexture* VulkanRenderer::CreateBlankTexture(std::string name)
	{
		VulkanTexture* outTex = nullptr;
		if (HasTexture(name, &outTex))
			return outTex;
		
		VulkanTexture blank = VulkanTexture(this);
		// set name after creating as creating blank will set name to "blank"
		blank.Name = name;
		UploadTexture(blank);
		outTex = &Textures[name];
		return outTex;
	}
	
	
	VulkanMaterial* VulkanRenderer::CreateMaterial(std::string name, Texture* texture, std::string fragShaderPath, std::string vertShaderPath)
	{
		VkShaderModule fragShader;
		if (!LoadShaderModule(fragShaderPath.c_str(), &fragShader))
		{
			LOGCORE_WARN("VULKAN: Failed to load frag shader: {0}", fragShaderPath);
		}
		
		VkShaderModule vertShader;
		if (!LoadShaderModule(vertShaderPath.c_str(), &vertShader))
		{
			LOGCORE_WARN("VULKAN: Failed to load frag shader: {0}", vertShaderPath);
		}
		
		PipelineBuilder pipelineBuilder;
		pipelineBuilder.ShaderStages.push_back(VkInit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertShader));
		pipelineBuilder.ShaderStages.push_back(VkInit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragShader));
		
		
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = VkInit::PipelineLayoutCreateInfo();
		
		VkPushConstantRange pushConstant;
		pushConstant.offset = 0;
		pushConstant.size = sizeof(MeshPushConstants);
		pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		
		pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		
		VkDescriptorSetLayout setLayouts[] = { m_globalSetLayout, m_objectSetLayout, m_singleTextureSetLayout };
		
		pipelineLayoutInfo.setLayoutCount = 3;
		pipelineLayoutInfo.pSetLayouts = setLayouts;
		
		VkPipelineLayout pipelineLayout;
		
		VK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &pipelineLayout));
		
		
		pipelineBuilder.VertexInputInfo = VkInit::VertexInputCreateInfo();
		
		pipelineBuilder.InputAssembly = VkInit::InputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		
		pipelineBuilder.Viewport.x = 0.0f;
		pipelineBuilder.Viewport.y = 0.0f;
		pipelineBuilder.Viewport.width = (float)WindowExtent.width;
		pipelineBuilder.Viewport.height = (float)WindowExtent.height;
		pipelineBuilder.Viewport.minDepth = 0.0f;
		pipelineBuilder.Viewport.maxDepth = 1.0f;
		
		pipelineBuilder.Scissor.offset = { 0, 0 };
		pipelineBuilder.Scissor.extent = WindowExtent;
		
		pipelineBuilder.Rasterizer = VkInit::RasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
		
		pipelineBuilder.Multisampling = VkInit::MultisamlingStateCreateInfo();
		
		pipelineBuilder.ColourBlendAttachment = VkInit::ColourBlendAttachmentState();
		
		pipelineBuilder.PipelineLayout = pipelineLayout;
		
		VertexInputDescription vertexDescription = VulkanVertex::GetVertexDescription();
		
		pipelineBuilder.VertexInputInfo.pVertexAttributeDescriptions = vertexDescription.Attributes.data();
		pipelineBuilder.VertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)vertexDescription.Attributes.size();
		
		pipelineBuilder.VertexInputInfo.pVertexBindingDescriptions = vertexDescription.Bindings.data();
		pipelineBuilder.VertexInputInfo.vertexBindingDescriptionCount = (uint32_t)vertexDescription.Bindings.size();
		
		pipelineBuilder.DepthStencil = VkInit::DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
		
		VkPipeline pipeline = pipelineBuilder.BuildPipeline(m_device, m_renderPass);
		
		VulkanMaterial material(pipeline, pipelineLayout, name, fragShader, vertShader, this);
		material.SetTexture(texture);
		UploadMaterial(material);
		
		return &Materials[name];
	}
	
	
	void VulkanRenderer::Init()
	{
		WindowExtent = { pWindow->GetWidth(), pWindow->GetHeight() };
		InitVulkan();
		InitSwapchain();
		InitDefaultRenderPass();
		InitFrameBuffers();
		InitCommands();
		InitSyncStructures();
		InitDescriptors();
		InitDefaultPipeline();
		InitSamplers();
		
		LOGCORE_INFO("VULKAN RENDERER INITIALISED");
		
		LoadDefaultMeshes();
		
		IsInitialized = true;
	}
	
	
	void VulkanRenderer::Cleanup()
	{
		if (IsInitialized)
		{
			vkDeviceWaitIdle(m_device);
			
			vkWaitForFences(m_device, 1, &GetCurrentFrame().RenderFence, true, 1000000000);
			
			CleanupSwapchain();
			
			MainDeletionQueue.flush();
			
			vmaDestroyAllocator(m_allocator);
			
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
			
			vkDestroyDevice(m_device, nullptr);
			vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
			vkDestroyInstance(m_instance, nullptr);
		}
	}
	
	
	VulkanMaterial& VulkanRenderer::RecreateMaterial(VulkanMaterial& mat)
	{
		PipelineBuilder pipelineBuilder;
		pipelineBuilder.ShaderStages.push_back(VkInit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, mat.VertShader));
		pipelineBuilder.ShaderStages.push_back(VkInit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, mat.FragShader));
		
		
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = VkInit::PipelineLayoutCreateInfo();
		
		VkPushConstantRange pushConstant;
		pushConstant.offset = 0;
		pushConstant.size = sizeof(MeshPushConstants);
		pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		
		pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		
		VkDescriptorSetLayout setLayouts[] = { m_globalSetLayout, m_objectSetLayout, m_singleTextureSetLayout };
		
		pipelineLayoutInfo.setLayoutCount = 3;
		pipelineLayoutInfo.pSetLayouts = setLayouts;
		
		VkPipelineLayout pipelineLayout;
		
		VK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &pipelineLayout));
		
		
		pipelineBuilder.VertexInputInfo = VkInit::VertexInputCreateInfo();
		
		pipelineBuilder.InputAssembly = VkInit::InputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		
		pipelineBuilder.Viewport.x = 0.0f;
		pipelineBuilder.Viewport.y = 0.0f;
		pipelineBuilder.Viewport.width = (float)WindowExtent.width;
		pipelineBuilder.Viewport.height = (float)WindowExtent.height;
		pipelineBuilder.Viewport.minDepth = 0.0f;
		pipelineBuilder.Viewport.maxDepth = 1.0f;
		
		pipelineBuilder.Scissor.offset = { 0, 0 };
		pipelineBuilder.Scissor.extent = WindowExtent;
		
		pipelineBuilder.Rasterizer = VkInit::RasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
		
		pipelineBuilder.Multisampling = VkInit::MultisamlingStateCreateInfo();
		
		pipelineBuilder.ColourBlendAttachment = VkInit::ColourBlendAttachmentState();
		
		pipelineBuilder.PipelineLayout = pipelineLayout;
		
		VertexInputDescription vertexDescription = VulkanVertex::GetVertexDescription();
		
		pipelineBuilder.VertexInputInfo.pVertexAttributeDescriptions = vertexDescription.Attributes.data();
		pipelineBuilder.VertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)vertexDescription.Attributes.size();
		
		pipelineBuilder.VertexInputInfo.pVertexBindingDescriptions = vertexDescription.Bindings.data();
		pipelineBuilder.VertexInputInfo.vertexBindingDescriptionCount = (uint32_t)vertexDescription.Bindings.size();
		
		pipelineBuilder.DepthStencil = VkInit::DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
		
		VkPipeline pipeline = pipelineBuilder.BuildPipeline(m_device, m_renderPass);
		
		VulkanMaterial material(pipeline, pipelineLayout, mat.Name, mat.FragShader, mat.VertShader, this);
		material.SetTexture(mat.pTexture);
		UploadMaterial(material);
		
		return Materials[mat.Name];
	}
	
	
	void VulkanRenderer::RecreateMaterials()
	{
		for (auto& it : Materials)
		{
			// NOTE: probably should find a better solution to this, rather than checking if it's the default every itteration
			if (it.first != "default")
				it.second = RecreateMaterial(it.second);
		}
	}
	
	
	void VulkanRenderer::CleanupSwapchain()
	{
		for (auto framebuffer : m_framebuffers)
		{
			vkDestroyFramebuffer(m_device, framebuffer, nullptr);
		}
		
		for (auto mat : Materials)
		{
			vkDestroyPipeline(m_device, mat.second.Pipeline, nullptr);
			vkDestroyPipelineLayout(m_device, mat.second.PipelineLayout, nullptr);
		}
		
		vkDestroyRenderPass(m_device, m_renderPass, nullptr);
		
		
		for (int i = 0; i < m_swapchainImages.size(); ++i)
		{
			vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
		}
		
		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
		
		vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
		
		for (int i = 0; i < FRAME_OVERLAP; ++i)
		{
			vkDestroyFence(m_device, Frames[i].RenderFence, nullptr);
			vkDestroySemaphore(m_device, Frames[i].PresentSemaphore, nullptr);
			vkDestroySemaphore(m_device, Frames[i].RenderSemaphore, nullptr);
		}
		
		vkDestroyFence(m_device, UploadContext.UploadFence, nullptr);
	}
	
	
	void VulkanRenderer::RecreateSwapchain()
	{
		vkDeviceWaitIdle(m_device);
		
		CleanupSwapchain();
		
		WindowExtent = { pWindow->GetWidth(), pWindow->GetHeight() };
		
		InitSwapchain();
		InitDefaultRenderPass();
		InitFrameBuffers();
		InitDescriptors();
		InitDefaultPipeline();
		InitSyncStructures();
		
		RecreateMaterials();
	}
	
	
	void VulkanRenderer::InitImgui()
	{
		//1: create descriptor pool for IMGUI
		// the size of the pool is very oversize, but it's copied from imgui demo itself.
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		
		VkDescriptorPool imguiPool;
		VK_CHECK(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &imguiPool));
		
		
		// 2: initialize imgui library
		
		//this initializes the core structures of imgui
		ImGui::CreateContext();
		
		//this initializes imgui for SDL
		ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)pWindow->GetNativeWindow(), true);
		
		//this initializes imgui for Vulkan
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = m_instance;
		init_info.PhysicalDevice = m_physicalDevice;
		init_info.Device = m_device;
		init_info.Queue = m_graphicsQueue;
		init_info.DescriptorPool = imguiPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		
		ImGui_ImplVulkan_Init(&init_info, m_renderPass);
		
		//execute a gpu command to upload imgui font textures
		ImmediateSubmit([&](VkCommandBuffer cmd) {
							ImGui_ImplVulkan_CreateFontsTexture(cmd);
						});
		
		//clear font textures from cpu data
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		
		//add the destroy the imgui created structures
		MainDeletionQueue.push_function([=]() {
											
											vkDestroyDescriptorPool(m_device, imguiPool, nullptr);
											ImGui_ImplVulkan_Shutdown();
										});
	}
	
	
	void VulkanRenderer::InitVulkan()
	{
		vkb::InstanceBuilder builder;
		
		auto instRet = builder.set_app_name("ChaosEngine")
			.request_validation_layers(USE_VALIDATION_LAYERS)
			.require_api_version(1, 1, 0)
			.use_default_debug_messenger()
			.build();
		
		vkb::Instance vkbInst = instRet.value();
		
		m_instance = vkbInst.instance;
		m_debugMessenger = vkbInst.debug_messenger;
		
		
		//																																				TODO: Make platform agnostic
		if (glfwCreateWindowSurface(m_instance, (GLFWwindow*)pWindow->GetNativeWindow(), nullptr, &m_surface) != VK_SUCCESS)
		{
			LOGCORE_ERROR("VULKAN: failed to create window surface!");
		}
		
		vkb::PhysicalDeviceSelector selector{ vkbInst };
		vkb::PhysicalDevice physicalDevice = selector
			.set_minimum_version(1, 1)
			.set_surface(m_surface)
			.select()
			.value();
		
		vkb::DeviceBuilder deviceBuilder{ physicalDevice };
		vkb::Device vkbDevice = deviceBuilder.build().value();
		
		m_device = vkbDevice.device;
		m_physicalDevice = physicalDevice.physical_device;
		
		m_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		m_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
		
		VmaAllocatorCreateInfo allocInfo = {};
		allocInfo.physicalDevice = m_physicalDevice;
		allocInfo.device = m_device;
		allocInfo.instance = m_instance;
		vmaCreateAllocator(&allocInfo, &m_allocator);
		
		vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);
		
		LOGCORE_INFO("GPU min buffer alignment: {0}", m_physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
	}
	
	
	void VulkanRenderer::InitSwapchain()
	{		
		// TODO: use present mode based on Vsync variable
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR; // VK_PRESENT_MODE_MAILBOX_KHR // mailbox == vsync off
		
		vkb::SwapchainBuilder swapchainBuilder(m_physicalDevice, m_device, m_surface);
		vkb::Swapchain vkbSwapchain = swapchainBuilder
			.use_default_format_selection()
			.set_desired_present_mode(presentMode)
			.set_desired_extent(WindowExtent.width, WindowExtent.height)
			.build()
			.value();
		
		m_swapchain = vkbSwapchain.swapchain;
		m_swapchainImages = vkbSwapchain.get_images().value();
		m_swapchainImageViews = vkbSwapchain.get_image_views().value();
		
		m_swapchainImageFormat = vkbSwapchain.image_format;
		
		VkExtent3D depthImageExtent = {
			WindowExtent.width,
			WindowExtent.height,
			1
		};
		
		m_depthFormat = VK_FORMAT_D32_SFLOAT;
		
		VkImageCreateInfo dimgInfo = VkInit::ImageCreateInfo(m_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);
		
		VmaAllocationCreateInfo dimgAllocInfo = {};
		dimgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		dimgAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
		vmaCreateImage(m_allocator, &dimgInfo, &dimgAllocInfo, &m_depthImage.Image, &m_depthImage.Allocation, nullptr);
		
		VkImageViewCreateInfo dviewInfo = VkInit::ImageViewCreateInfo(m_depthFormat, m_depthImage.Image, VK_IMAGE_ASPECT_DEPTH_BIT);
		
		VK_CHECK(vkCreateImageView(m_device, &dviewInfo, nullptr, &m_depthImageView));
		
		SwapchainDeletionQueue.push_function([=]()
											 {
												 
												 vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
												 vkDestroyImageView(m_device, m_depthImageView, nullptr);
												 vmaDestroyImage(m_allocator, m_depthImage.Image, m_depthImage.Allocation);
												 
												 for (int i = 0; i < m_swapchainImageViews.size(); ++i)
												 {
													 vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
													 vkDestroyImage(m_device, m_swapchainImages[i], nullptr);
												 }
											 });
	}
	
	
	void VulkanRenderer::InitDefaultRenderPass()
	{
		VkAttachmentDescription colourAttachment = {};
		colourAttachment.format = m_swapchainImageFormat;
		colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		
		colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		
		colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		
		colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		
		VkAttachmentReference colourAttachmentRef = {};
		colourAttachmentRef.attachment = 0;
		colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		VkAttachmentDescription depthAttachment = {};
		depthAttachment.flags = 0;
		depthAttachment.format = m_depthFormat;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colourAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		
		VkAttachmentDescription attachments[2] = { colourAttachment, depthAttachment };
		
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 2;
		renderPassInfo.pAttachments = &attachments[0];
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;
		
		VK_CHECK(vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass));
		
		SwapchainDeletionQueue.push_function([=]()
											 {
												 vkDestroyRenderPass(m_device, m_renderPass, nullptr);
											 }
											 );
	}
	
	
	void VulkanRenderer::InitFrameBuffers()
	{
		VkFramebufferCreateInfo frameBufferInfo = VkInit::FramebufferCreateInfo(m_renderPass, WindowExtent);
		
		const uint32_t swapchainImageCount = (uint32_t)m_swapchainImages.size();
		m_framebuffers = std::vector<VkFramebuffer>(swapchainImageCount);
		
		for (uint32_t i = 0; i < swapchainImageCount; ++i)
		{
			VkImageView attachements[2] = { m_swapchainImageViews[i], m_depthImageView };
			frameBufferInfo.pAttachments = attachements;
			frameBufferInfo.attachmentCount = 2;
			VK_CHECK(vkCreateFramebuffer(m_device, &frameBufferInfo, nullptr, &m_framebuffers[i]));
			
			SwapchainDeletionQueue.push_function([=]()
												 {
													 vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
													 vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
												 });
		}
	}
	
	
	void VulkanRenderer::InitCommands()
	{
		VkCommandPoolCreateInfo commandPoolInfo = VkInit::CommandPoolCreateInfo(m_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		
		for (int i = 0; i < FRAME_OVERLAP; ++i)
		{
			VK_CHECK(vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &Frames[i].CommandPool));
			
			VkCommandBufferAllocateInfo cmdAllocInfo = VkInit::CommandBufferAllocateInfo(Frames[i].CommandPool, 1);
			
			VK_CHECK(vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &Frames[i].MainCommandBuffer));
			
			MainDeletionQueue.push_function([=]()
											{
												vkDestroyCommandPool(m_device, Frames[i].CommandPool, nullptr);
											});
		}
		
		VkCommandPoolCreateInfo uploadCommandPoolInfo = VkInit::CommandPoolCreateInfo(m_graphicsQueueFamily);
		
		VK_CHECK(vkCreateCommandPool(m_device, &uploadCommandPoolInfo, nullptr, &UploadContext.CommandPool));
		
		MainDeletionQueue.push_function([=]()
										{
											vkDestroyCommandPool(m_device, UploadContext.CommandPool, nullptr);
										});
	}
	
	
	void VulkanRenderer::InitSyncStructures()
	{
		VkFenceCreateInfo fenceCreateInfo = VkInit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		VkSemaphoreCreateInfo semaphoreCreateInfo = VkInit::SemaphoreCreateInfo();
		
		
		for (int i = 0; i < FRAME_OVERLAP; ++i)
		{
			VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &Frames[i].RenderFence));
			VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &Frames[i].PresentSemaphore));
			VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &Frames[i].RenderSemaphore));
			
			MainDeletionQueue.push_function([=]()
											{
												vkDestroyFence(m_device, Frames[i].RenderFence, nullptr);
												vkDestroySemaphore(m_device, Frames[i].PresentSemaphore, nullptr);
												vkDestroySemaphore(m_device, Frames[i].RenderSemaphore, nullptr);
											});
		}
		
		VkFenceCreateInfo uploadFenceCreateInfo = VkInit::FenceCreateInfo();
		VK_CHECK(vkCreateFence(m_device, &uploadFenceCreateInfo, nullptr, &UploadContext.UploadFence));
		
		SwapchainDeletionQueue.push_function([=]() 
											 {
												 vkDestroyFence(m_device, UploadContext.UploadFence, nullptr);
											 });
	}
	
	
	void VulkanRenderer::InitDescriptors()
	{
		std::vector<VkDescriptorPoolSize> sizes =
		{
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
		};
		
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = 0;
		poolInfo.maxSets = 10;
		poolInfo.poolSizeCount = (uint32_t)sizes.size();
		poolInfo.pPoolSizes = sizes.data();
		
		vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool);
		
		VkDescriptorSetLayoutBinding camBinding = VkInit::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
		VkDescriptorSetLayoutBinding sceneBinding = VkInit::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1);
		
		VkDescriptorSetLayoutBinding bindings[] = { camBinding, sceneBinding };
		
		VkDescriptorSetLayoutCreateInfo setInfo = {};
		setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		setInfo.pNext = nullptr;
		
		setInfo.bindingCount = 2;
		setInfo.flags = 0;
		setInfo.pBindings = bindings;
		
		vkCreateDescriptorSetLayout(m_device, &setInfo, nullptr, &m_globalSetLayout);
		
		
		VkDescriptorSetLayoutBinding objectBind = VkInit::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
		
		VkDescriptorSetLayoutCreateInfo set2Info = {};
		set2Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		set2Info.bindingCount = 1;
		set2Info.flags = 0;
		set2Info.pNext = nullptr;
		set2Info.pBindings = &objectBind;
		
		vkCreateDescriptorSetLayout(m_device, &set2Info, nullptr, &m_objectSetLayout);
		
		VkDescriptorSetLayoutBinding textureBind = VkInit::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);
		
		VkDescriptorSetLayoutCreateInfo set3Info = {};
		set3Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		set3Info.bindingCount = 1;
		set3Info.flags = 0;
		set3Info.pNext = nullptr;
		set3Info.pBindings = &textureBind;
		
		vkCreateDescriptorSetLayout(m_device, &set3Info, nullptr, &m_singleTextureSetLayout);
		
		const size_t sceneParamBufferSize = FRAME_OVERLAP * PadUniformBufferSize(sizeof(GPUSceneData));
		
		m_sceneParameterBuffer = CreateBuffer(sceneParamBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		
		for (int i = 0; i < FRAME_OVERLAP; ++i)
		{
			Frames[i].CameraBuffer = CreateBuffer(sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
			
			Frames[i].ObjectBuffer = CreateBuffer(sizeof(GPUObjectData) * MAX_OBJECTS, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
			
			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.pNext = nullptr;
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_descriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &m_globalSetLayout;
			
			vkAllocateDescriptorSets(m_device, &allocInfo, &Frames[i].GlobalDescriptor);
			
			VkDescriptorSetAllocateInfo objectSetAlloc = {};
			objectSetAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			objectSetAlloc.pNext = nullptr;
			objectSetAlloc.descriptorPool = m_descriptorPool;
			objectSetAlloc.descriptorSetCount = 1;
			objectSetAlloc.pSetLayouts = &m_objectSetLayout;
			
			vkAllocateDescriptorSets(m_device, &objectSetAlloc, &Frames[i].ObjectDescriptor);
			
			VkDescriptorBufferInfo camInfo;
			camInfo.buffer = Frames[i].CameraBuffer.Buffer;
			camInfo.offset = 0;
			camInfo.range = sizeof(GPUCameraData);
			
			VkDescriptorBufferInfo sceneInfo;
			sceneInfo.buffer = m_sceneParameterBuffer.Buffer;
			sceneInfo.offset = 0;
			sceneInfo.range = sizeof(GPUSceneData);
			
			VkDescriptorBufferInfo objectBufferInfo;
			objectBufferInfo.buffer = Frames[i].ObjectBuffer.Buffer;
			objectBufferInfo.offset = 0;
			objectBufferInfo.range = sizeof(GPUObjectData) * MAX_OBJECTS;
			
			VkWriteDescriptorSet camWrite = VkInit::WriteDescriptorBuffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Frames[i].GlobalDescriptor, &camInfo, 0);
			
			VkWriteDescriptorSet sceneWrite = VkInit::WriteDescriptorBuffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, Frames[i].GlobalDescriptor, &sceneInfo, 1);
			
			VkWriteDescriptorSet objectWrite = VkInit::WriteDescriptorBuffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, Frames[i].ObjectDescriptor, &objectBufferInfo, 0);
			
			VkWriteDescriptorSet setWrites[] = { camWrite, sceneWrite, objectWrite };
			
			vkUpdateDescriptorSets(m_device, 3, setWrites, 0, nullptr);
		}
		
		MainDeletionQueue.push_function([&]()
										{
											vmaDestroyBuffer(m_allocator, m_sceneParameterBuffer.Buffer, m_sceneParameterBuffer.Allocation);
											vkDestroyDescriptorSetLayout(m_device, m_globalSetLayout, nullptr);
											vkDestroyDescriptorSetLayout(m_device, m_objectSetLayout, nullptr);
											vkDestroyDescriptorSetLayout(m_device, m_singleTextureSetLayout, nullptr);
											
											vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
											
											for (int i = 0; i < FRAME_OVERLAP; ++i)
											{
												vmaDestroyBuffer(m_allocator, Frames[i].CameraBuffer.Buffer, Frames[i].CameraBuffer.Allocation);
												vmaDestroyBuffer(m_allocator, Frames[i].ObjectBuffer.Buffer, Frames[i].ObjectBuffer.Allocation);
											}
										});
	}
	
	
	void VulkanRenderer::InitDefaultPipeline()
	{
		VkShaderModule fragShader;
		if (!LoadShaderModule("../ChaosEngine/shaders/spv/default_lit.frag.spv", &fragShader))
		{
			LOGCORE_WARN("Failed to load triangle frag shader module");
		}
		
		VkShaderModule vertShader;
		if (!LoadShaderModule("../ChaosEngine/shaders/spv/default.vert.spv", &vertShader))
		{
			LOGCORE_WARN("Failed to load triangle vert shader module");
		}
		
		PipelineBuilder pipelineBuilder;
		pipelineBuilder.ShaderStages.push_back(VkInit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertShader));
		pipelineBuilder.ShaderStages.push_back(VkInit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragShader));
		
		
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = VkInit::PipelineLayoutCreateInfo();
		
		VkPushConstantRange pushConstant;
		pushConstant.offset = 0;
		pushConstant.size = sizeof(MeshPushConstants);
		pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		
		pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		
		VkDescriptorSetLayout setLayouts[] = { m_globalSetLayout, m_objectSetLayout };
		
		pipelineLayoutInfo.setLayoutCount = 2;
		pipelineLayoutInfo.pSetLayouts = setLayouts;
		
		VkPipelineLayout pipelineLayout;
		
		VK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &pipelineLayout));
		
		
		pipelineBuilder.VertexInputInfo = VkInit::VertexInputCreateInfo();
		
		pipelineBuilder.InputAssembly = VkInit::InputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		
		pipelineBuilder.Viewport.x = 0.0f;
		pipelineBuilder.Viewport.y = 0.0f;
		pipelineBuilder.Viewport.width = (float)WindowExtent.width;
		pipelineBuilder.Viewport.height = (float)WindowExtent.height;
		pipelineBuilder.Viewport.minDepth = 0.0f;
		pipelineBuilder.Viewport.maxDepth = 1.0f;
		
		pipelineBuilder.Scissor.offset = { 0, 0 };
		pipelineBuilder.Scissor.extent = WindowExtent;
		
		pipelineBuilder.Rasterizer = VkInit::RasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
		
		pipelineBuilder.Multisampling = VkInit::MultisamlingStateCreateInfo();
		
		pipelineBuilder.ColourBlendAttachment = VkInit::ColourBlendAttachmentState();
		
		pipelineBuilder.PipelineLayout = pipelineLayout;
		
		VertexInputDescription vertexDescription = VulkanVertex::GetVertexDescription();
		
		pipelineBuilder.VertexInputInfo.pVertexAttributeDescriptions = vertexDescription.Attributes.data();
		pipelineBuilder.VertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)vertexDescription.Attributes.size();
		
		pipelineBuilder.VertexInputInfo.pVertexBindingDescriptions = vertexDescription.Bindings.data();
		pipelineBuilder.VertexInputInfo.vertexBindingDescriptionCount = (uint32_t)vertexDescription.Bindings.size();
		
		pipelineBuilder.DepthStencil = VkInit::DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
		
		VkPipeline pipeline = pipelineBuilder.BuildPipeline(m_device, m_renderPass);
		
		VulkanMaterial defaultMat(pipeline, pipelineLayout, "default", fragShader, vertShader, this);
		UploadMaterial(defaultMat);
	}
	
	
	void VulkanRenderer::InitSamplers()
	{
		VkSamplerCreateInfo nearestNeighbourSamplerInfo = VkInit::SamplerCreateInfo(VK_FILTER_NEAREST);
		
		vkCreateSampler(m_device, &nearestNeighbourSamplerInfo, nullptr, &m_nearestNeighbourSampler);
	}
	
	
	AllocatedBuffer VulkanRenderer::CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = nullptr;
		
		bufferInfo.size = allocSize;
		bufferInfo.usage = usage;
		
		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = memoryUsage;
		
		AllocatedBuffer newBuffer;
		
		VK_CHECK(vmaCreateBuffer(m_allocator, &bufferInfo, &vmaallocInfo,
								 &newBuffer.Buffer,
								 &newBuffer.Allocation,
								 nullptr));
		
		return newBuffer;
	}
	
	
	bool VulkanRenderer::LoadShaderModule(const char* filePath, VkShaderModule* outShaderModule)
	{
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);
		
		if (!file.is_open())
			return false;
		
		size_t fileSize = (size_t)file.tellg();
		
		std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
		
		file.seekg(0);
		
		file.read((char*)buffer.data(), fileSize);
		
		file.close();
		
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext = nullptr;
		
		createInfo.codeSize = buffer.size() * sizeof(uint32_t);
		createInfo.pCode = buffer.data();
		
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			return false;
		}
		*outShaderModule = shaderModule;
		return true;
	}
	
	
	void VulkanRenderer::InitDefaultMaterials()
	{
		
	}
	
	
	// loads basic meshes like tri and quad for general use
	void VulkanRenderer::LoadDefaultMeshes()
	{
		VulkanMesh triMesh{};
		triMesh.Vertices.resize(3);
		
		triMesh.Vertices[0].Position = { 1.f, 1.f, 0.f };
		triMesh.Vertices[1].Position = { -1.f, 1.f, 0.f };
		triMesh.Vertices[2].Position = { 0.f, -1.f, 0.f };
		
		triMesh.Vertices[0].Colour = { 0.f, 1.f, 0.f };
		triMesh.Vertices[1].Colour = { 0.f, 1.f, 0.f };
		triMesh.Vertices[2].Colour = { 0.f, 1.f, 0.f };
		
		Vec3 a = triMesh.Vertices[0].Position - triMesh.Vertices[1].Position;
		Vec3 b = triMesh.Vertices[1].Position - triMesh.Vertices[2].Position;
		
		Vec3 Normal = Vec3::Cross(a, b).Normalised() * -1.0f;
		
		triMesh.Vertices[0].Normal = { 1.0f, 1.0f, 0.0f };
		triMesh.Vertices[1].Normal = { 1.0f, 1.0f, 0.0f };
		triMesh.Vertices[2].Normal = { 1.0f, 1.0f, 0.0f };
		
		
		VulkanMesh quadMesh{};
		quadMesh.Vertices.resize(6);
		
		for (int i = 0; i < 6; ++i)
		{
			quadMesh.Vertices[i].Position = QUAD_VERTEX_POSITIONS[i];
			quadMesh.Vertices[i].Colour = {1.0f, 1.0f, 1.0f};
			quadMesh.Vertices[i].Normal = {0.0f, 0.0f, 1.0f};
		}
		
		UploadMesh(triMesh);
		UploadMesh(quadMesh);
		
		Meshes["triangle"] = triMesh;
		Meshes["quad"] = quadMesh;
	}
	
	
	void VulkanRenderer::UploadMesh(VulkanMesh& mesh)
	{
		const size_t bufferSize = mesh.Vertices.size() * sizeof(Vertex);
		
		VkBufferCreateInfo stagingBufferInfo = {};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferInfo.pNext = nullptr;
		
		stagingBufferInfo.size = bufferSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		
		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		
		AllocatedBuffer stagingBuffer;
		
		VK_CHECK(vmaCreateBuffer(m_allocator, &stagingBufferInfo, &vmaallocInfo,
								 &stagingBuffer.Buffer,
								 &stagingBuffer.Allocation,
								 nullptr));
		
		void* data;
		vmaMapMemory(m_allocator, stagingBuffer.Allocation, &data);
		
		memcpy(data, mesh.Vertices.data(), mesh.Vertices.size() * sizeof(Vertex));
		
		vmaUnmapMemory(m_allocator, stagingBuffer.Allocation);
		
		VkBufferCreateInfo vertexBufferInfo = {};
		vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexBufferInfo.pNext = nullptr;
		vertexBufferInfo.size = bufferSize;
		vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		
		vmaallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		
		VK_CHECK(vmaCreateBuffer(m_allocator, &vertexBufferInfo, &vmaallocInfo,
								 &mesh.VertexBuffer.Buffer,
								 &mesh.VertexBuffer.Allocation,
								 nullptr));
		
		ImmediateSubmit([=](VkCommandBuffer cmd)
						{
							VkBufferCopy copy;
							copy.dstOffset = 0;
							copy.srcOffset = 0;
							copy.size = bufferSize;
							vkCmdCopyBuffer(cmd, stagingBuffer.Buffer, mesh.VertexBuffer.Buffer, 1, &copy);
						});
		
		MainDeletionQueue.push_function([=]()
										{
											vmaDestroyBuffer(m_allocator, mesh.VertexBuffer.Buffer, mesh.VertexBuffer.Allocation);
										});
		
		vmaDestroyBuffer(m_allocator, stagingBuffer.Buffer, stagingBuffer.Allocation);
	}
	
	
	void VulkanRenderer::UploadMaterial(VulkanMaterial& mat)
	{
		Materials[mat.Name] = mat;
	}
	
	
	void VulkanRenderer::UploadTexture(VulkanTexture& tex)
	{
		Textures[tex.Name] = tex;
	}
	
	
	void VulkanRenderer::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
	{
		VkCommandBufferAllocateInfo cmdAllocInfo = VkInit::CommandBufferAllocateInfo(UploadContext.CommandPool, 1);
		
		VkCommandBuffer cmd;
		VK_CHECK(vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &cmd));
		
		VkCommandBufferBeginInfo cmdBeginInfo = VkInit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		
		VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
		
		function(cmd);
		
		VK_CHECK(vkEndCommandBuffer(cmd));
		
		VkSubmitInfo submit = VkInit::SubmitInfo(&cmd);
		
		VK_CHECK(vkQueueSubmit(m_graphicsQueue, 1, &submit, UploadContext.UploadFence));
		
		vkWaitForFences(m_device, 1, &UploadContext.UploadFence, true, 9999999999);
		vkResetFences(m_device, 1, &UploadContext.UploadFence);
		
		vkResetCommandPool(m_device, UploadContext.CommandPool, 0);
	}
	
	
	VulkanMaterial* VulkanRenderer::GetMaterial(const std::string& name)
	{
		auto it = Materials.find(name);
		if (it == Materials.end())
		{
			return nullptr;
		}
		
		return &(*it).second;
	}
	
	
	VulkanMesh* VulkanRenderer::GetMesh(const std::string& name)
	{
		auto it = Meshes.find(name);
		if (it == Meshes.end())
		{
			return nullptr;
		}
		
		return &(*it).second;
	}
	
	
	FrameData& VulkanRenderer::GetCurrentFrame()
	{
		return Frames[FrameNumber % FRAME_OVERLAP];
	}
	
	
	size_t VulkanRenderer::PadUniformBufferSize(size_t originalSize)
	{
		size_t minUboAlignment = m_physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
		size_t alignedSize = originalSize;
		if (minUboAlignment > 0)
		{
			alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
		}
		
		return alignedSize;
	}
	
	
	VkPipeline PipelineBuilder::BuildPipeline(VkDevice device, VkRenderPass pass)
	{
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;
		
		viewportState.viewportCount = 1;
		viewportState.pViewports = &Viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &Scissor;
		
		VkPipelineColorBlendStateCreateInfo colourBlending = {};
		colourBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colourBlending.pNext = nullptr;
		
		colourBlending.logicOpEnable = VK_FALSE;
		colourBlending.logicOp = VK_LOGIC_OP_COPY;
		colourBlending.attachmentCount = 1;
		colourBlending.pAttachments = &ColourBlendAttachment;
		
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = nullptr;
		
		pipelineInfo.stageCount = (uint32_t)ShaderStages.size();
		pipelineInfo.pStages = ShaderStages.data();
		pipelineInfo.pVertexInputState = &VertexInputInfo;
		pipelineInfo.pInputAssemblyState = &InputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &Rasterizer;
		pipelineInfo.pMultisampleState = &Multisampling;
		pipelineInfo.pColorBlendState = &colourBlending;
		pipelineInfo.pDepthStencilState = &DepthStencil;
		pipelineInfo.layout = PipelineLayout;
		pipelineInfo.renderPass = pass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		
		pipelineInfo.pDepthStencilState = &DepthStencil;
		
		VkPipeline newPipeline;
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS)
		{
			LOGCORE_WARN("VULKAN: failed to create pipeline");
			return VK_NULL_HANDLE;
		}
		
		return newPipeline;
	}
}