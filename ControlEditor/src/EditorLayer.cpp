#include "EditorLayer.h"
#pragma once
#include <ImGUI/imgui.h>
#include "ImGUI/examples/imgui_impl_vulkan.h"
#include "ImGUI/examples/imgui_impl_glfw.h"
#include "Chaos/Core/Application.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanTexture.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif // !GLFW_INCLUDE_NONE
#include <GLFW/include/GLFW/glfw3.h>
#include <GLM/glm/glm.hpp>

//temp
#include <Chaos/Entity/Components/Render.h>
namespace Chaos
{
	EditorLayer::EditorLayer()
	{
		m_debugName = "ControlEditorLayer";
	}
	void EditorLayer::OnAttach()
	{
		VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(Application::Get().GetRenderer());
		m_veiwportImageId = (ImTextureID)ImGui_ImplVulkan_AddTexture(renderer.GetTexSampler(), renderer.GetRenderedFrames()[renderer.GetCurrentFrame()], VK_IMAGE_LAYOUT_UNDEFINED);

		entity.AddComponent<Render>();
		entity.GetTransform()->Position() = { 0,0 };
		entity.GetTransform()->Scale() = Vec2(1, 1);
		entity.GetTransform()->Position() = Vec2(Application::Get().GetMainCamera()->GetPosition().X, Application::Get().GetMainCamera()->GetPosition().Y);

		LOGCORE_INFO("entity: {0},{1} Camera: {2},{3},{4}", entity.GetTransform()->Position().X,
			entity.GetTransform()->Position().Y,
			Application::Get().GetMainCamera()->GetPosition().X,
			Application::Get().GetMainCamera()->GetPosition().Y,
			Application::Get().GetMainCamera()->GetPosition().Z);
	}
	void EditorLayer::OnDetach()
	{
	}
	void EditorLayer::OnUpdate(float deltaTime)
	{
		m_time = deltaTime;
		entity.Update();
	}
	void EditorLayer::OnEvent(Event& event)
	{
	}
	void EditorLayer::OnImGuiUpdate()
	{
		static bool show = true;
		//ImGui::ShowDemoWindow(&show);
		VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(Application::Get().GetRenderer());

#if !CHAOS_RELEASE
		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit")) Chaos::Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::Text("FPS: %f", 1 / m_time);
		ImGui::Text("Quads: %d", renderer.GetDebugInfo().TotalQuadsDrawn);
		ImGui::Text("Draw calls: %d", renderer.GetDebugInfo().NumOfDrawCalls);
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("viewport");

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		if (m_veiwportImageId != nullptr)
		{
			ImGui::Image(m_veiwportImageId, { viewportSize.x, viewportSize.y }, ImVec2{ 0, -1 }, ImVec2{ 1, 0 });
		}
		ImGui::End();
		ImGui::PopStyleVar();
#endif
		// FIXME-VIEWPORT: Select a default viewport
		const float DISTANCE = 10.0f;
		static int corner = 2;
		if (corner != -1)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 window_pos = ImVec2((corner & 1) ? (viewport->Pos.x + viewport->Size.x - DISTANCE) : (viewport->Pos.x + DISTANCE), (corner & 2) ? (viewport->Pos.y + viewport->Size.y - DISTANCE) : (viewport->Pos.y + DISTANCE));
			ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowViewport(viewport->ID);
		}
		//ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		if (ImGui::Begin("FPS counter", &show, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			ImGui::Text("FPS: %f", 1 / m_time);
			ImGui::Text("Quads: %d", renderer.GetDebugInfo().TotalQuadsDrawn);
			ImGui::Text("Draw calls: %d", renderer.GetDebugInfo().NumOfDrawCalls);

			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
				if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
				if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
				if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
				if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
				if (&show && ImGui::MenuItem("Close")) show = false;
				ImGui::EndPopup();
			}
		}
		ImGui::End();


		renderer.GetDebugInfo().TotalQuadsDrawn = 0;
	}
}