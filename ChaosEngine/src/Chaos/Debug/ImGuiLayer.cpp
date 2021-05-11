#include "chaospch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include "ImGUI/examples/imgui_impl_vulkan.h"
#include "ImGUI/examples/imgui_impl_glfw.h"


namespace Chaos
{
	bool ImGuiLayer::s_ImGuiInited = false;
	
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
}
