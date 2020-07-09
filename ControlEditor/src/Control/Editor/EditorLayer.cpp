#include "EditorLayer.h"
#pragma once
#include <ImGUI/imgui.h>
#include "ImGUI/examples/imgui_impl_vulkan.h"
#include "ImGUI/examples/imgui_impl_glfw.h"
#include "Chaos/Core/Application.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanTexture.h"
#include "Chaos/Core/Scene.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif // !GLFW_INCLUDE_NONE
#include <GLFW/include/GLFW/glfw3.h>
#include <GLM/glm/glm.hpp>


//temp
#include <Chaos/Entity/Components/Render.h>
namespace Chaos
{
	EditorLayer::EditorLayer() : m_cameraController(*Application::Get().GetMainCameraEntity()->GetTransform(), *Application::Get().GetMainCamera())
	{
		m_debugName = "ControlEditorLayer";
	}
	void EditorLayer::OnAttach()
	{
		m_scene.StartScene();

		VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(Application::Get().GetRenderer());
		m_veiwportImageId = (ImTextureID)ImGui_ImplVulkan_AddTexture(renderer.GetTexSampler(), renderer.GetRenderedFrames()[renderer.GetCurrentFrame()], VK_IMAGE_LAYOUT_UNDEFINED);
	}
	void EditorLayer::OnDetach()
	{
		m_scene.EndScene();
	}
	void EditorLayer::OnUpdate(float deltaTime)
	{
		m_scene.Update();
		m_time = deltaTime;
		m_cameraController.Update(deltaTime, m_ViewportSize, m_viewportFocused);
	}
	void EditorLayer::OnEvent(Event& event)
	{
		m_cameraController.OnEvent(event, m_viewportHovered);
	}
	void EditorLayer::OnImGuiUpdate()
	{
		static bool show = true;
		//ImGui::ShowDemoWindow(&show);

		CreateDocker();
		CreateViewPort();
		CreateSceneHierarchy();
		CreateSettings();
		CreateInspector();

		if (m_profilerOpen)
			CreateProfiler();

	}


	void EditorLayer::CreateDocker()
	{
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

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Profiler"))
				{
					m_profilerOpen ? m_profilerOpen = false : m_profilerOpen = true; //toggles profiler bool
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::End();
	}

	void EditorLayer::CreateSceneHierarchy()
	{
		//Listing entities in editor scene pannel
		ImGui::Begin("Scene");

		static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		for (size_t i = 0; i < m_scene.GetEntities().size(); ++i)
		{
			bool nodeClicked = false;
			auto entity = m_scene.GetEntities()[i];
			ImGuiTreeNodeFlags nodeFlags = baseFlags;
			if (IsSelected(entity))
				nodeFlags |= ImGuiTreeNodeFlags_Selected;

			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "%s", entity->GetName());
			if (ImGui::IsItemClicked())
				nodeClicked = true;

			if (nodeClicked)
			{
				if (ImGui::GetIO().KeyCtrl)
				{
					m_selectedEntities.push_back(entity);         // CTRL+click to toggle
				}
				else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
				{
					m_selectedEntities.clear();
					m_selectedEntities.push_back(entity);	// Click to single-select
				}
			}
		}

		ImGui::End();
	}

	void EditorLayer::CreateViewPort()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("viewport");

		m_ViewportSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

		if (m_veiwportImageId != nullptr)
		{
			ImGui::Image(m_veiwportImageId, { m_ViewportSize.X, m_ViewportSize.Y }, ImVec2{ 0, -1 }, ImVec2{ 1, 0 });
		}
		m_viewportFocused = ImGui::IsWindowFocused();
		m_viewportHovered = ImGui::IsWindowHovered();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::CreateSettings()
	{
		VulkanRenderer& renderer = dynamic_cast<VulkanRenderer&>(Application::Get().GetRenderer());
		ImGui::Begin("Stats");
		ImGui::Text("FPS: %f", 1 / m_time);
		ImGui::Text("Quads: %d", renderer.GetDebugInfo().TotalQuadsDrawn);
		ImGui::Text("Draw calls: %d", renderer.GetDebugInfo().NumOfDrawCalls);

		ImGui::End();

		renderer.GetDebugInfo().TotalQuadsDrawn = 0;
	}

	void EditorLayer::CreateInspector()
	{
		ImGui::Begin("Inspector");
		//if there is something selected (for now selects the last one to modify)
		if (m_selectedEntities.size() > 0)
		{
			Entity* entity = m_selectedEntities[m_selectedEntities.size() - 1];

			//Display name
			ImGui::InputText("Name", entity->GetName(), MAX_ENTITY_NAME_LENGTH);
			ImGui::Text("id: %d", entity->GetEntityID());
			ImGui::Separator();

			ImGui::Text("Transform");
			//display transform component
			float* pos[2] = { &entity->GetTransform()->Position().X, &entity->GetTransform()->Position().Y };
			float* rotation[2] = { &entity->GetTransform()->Rotation().X, &entity->GetTransform()->Rotation().Y };
			float* scale[2] = { &entity->GetTransform()->Scale().X, &entity->GetTransform()->Scale().Y };

			ImGui::Separator();
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { -2, 2 });

			ImGui::DragFloat2("Position", *pos, 0.01f);

			ImGui::DragFloat2("Rotation", *rotation, 1.0f, -180, 180);

			ImGui::DragFloat2("Scale", *scale, 0.01f);

			ImGui::PopStyleVar();

			ImGui::Separator();

			for (auto* component : entity->GetAllComponents())
			{
				//Should be generic, however right now I just wanted to implement some UI for changing sprites
				ImGui::Text(component->ToString());

				Render* render = dynamic_cast<Render*>(component);

				char filePath[128] = "";
				strcpy_s(filePath, render->GetTexture()->GetFilePath());

				if (ImGui::InputText("Texture Path", filePath, MAX_ENTITY_NAME_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					if (render->GetTexture() != Texture::GetBlank())
					{
						render->GetTexture()->Load(filePath);
					}
					else
					{
						render->SetTexture(Texture::Create(filePath));
					}
				}

				ImGui::Separator();
			}
			//for each component display info
		}
		ImGui::End();
	}

	void EditorLayer::CreateProfiler()
	{
		ImGui::Begin("Profiler");
		for (auto p : Timer::GetTimers())
		{
			ImGui::Text("%s : %f ms", p.first, p.second);
		}
		ImGui::End();
	}


	bool EditorLayer::IsSelected(Entity* entity)
	{
		for (auto e : m_selectedEntities)
		{
			if (e->GetEntityID() == entity->GetEntityID())
				return true;
		}
		return false;
	}
}