#include <chaospch.h>
#include <Chaos/Debug/ImGuiDebugInfo.h>
#include <Chaos/Debug/Console.h>

namespace Chaos
{
	ImGuiDebugInfo::ImGuiDebugInfo()
	{
		Console::AddCommand("debug", [&](){ m_showDebug ? m_showDebug = false : m_showDebug = true; });
	}
	
	void ImGuiDebugInfo::OnUpdate(float deltaTime)
	{
		m_fps = 1 / deltaTime;
	}
	
	void ImGuiDebugInfo::OnImGuiUpdate()
	{
		if (m_showDebug)
		{
			// FIXME-VIEWPORT: Select a default viewport
			const float DISTANCE = 10.0f;
			static int corner = 3;
			ImGuiIO& io = ImGui::GetIO();
			if (corner != -1)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImVec2 window_pos = ImVec2((corner & 1) ? (viewport->Pos.x + viewport->Size.x - DISTANCE) : (viewport->Pos.x + DISTANCE), (corner & 2) ? (viewport->Pos.y + viewport->Size.y - DISTANCE) : (viewport->Pos.y + DISTANCE));
				ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
				ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
				ImGui::SetNextWindowViewport(viewport->ID);
			}
			//ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
			if (ImGui::Begin("Debug overlay", &m_showDebug, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
			{
				ImGui::Text("Debug Info");
				ImGui::Separator();
				
				//Display info here..
				
				ImGui::Text("fps: %d", m_fps);
				
				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::MenuItem("Custom",       NULL, corner == -1)) corner = -1;
					if (ImGui::MenuItem("Top-left",     NULL, corner == 0)) corner = 0;
					if (ImGui::MenuItem("Top-right",    NULL, corner == 1)) corner = 1;
					if (ImGui::MenuItem("Bottom-left",  NULL, corner == 2)) corner = 2;
					if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
					if (&m_showDebug && ImGui::MenuItem("Close")) m_showDebug = false;
					ImGui::EndPopup();
				}
			}
			
			ImGui::End();
		}
		
	}
	
}