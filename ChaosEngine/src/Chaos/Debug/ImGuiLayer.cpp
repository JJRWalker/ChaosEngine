#include "chaospch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include "ImGUI/examples/imgui_impl_vulkan.h"
#include "ImGUI/examples/imgui_impl_glfw.h"
#include "Chaos/Core/Application.h"
#include "Platform/Vulkan/VulkanRenderer.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif // !GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <GLM/glm/glm.hpp>

#define VK_CHECK_RESULT(x) {if (x != VK_SUCCESS) { LOGCORE_ERROR("IMGUI: Failed to create vulkan object");}}

namespace Chaos
{
	bool ImGuiLayer::s_ImGuiInited = false;
	
	static void check_vk_result(VkResult err)
	{
		if (err == 0) return;
		LOGCORE_ERROR("IMGUI: vulkan error: {0}", err);
	}
	struct PushConstBlock {
		glm::vec2 scale;
		glm::vec2 translate;
	} pushConstBlock;
	
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}
	
	ImGuiLayer::~ImGuiLayer()
	{
	}
	
	void ImGuiLayer::OnAttach()
	{

	}
	
	void ImGuiLayer::OnDetach()
	{
	}
	
	void ImGuiLayer::Begin()
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	
	void ImGuiLayer::OnImGuiUpdate()
	{
		
	}
	
	void ImGuiLayer::End()
	{
		ImGui::Render();
		ImGui::EndFrame();
	}
	
	void ImGuiLayer::VulkanInit()
	{

	}
}
