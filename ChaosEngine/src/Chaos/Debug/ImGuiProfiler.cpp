#include "chaospch.h"

#include "ImGuiProfiler.h"
#include "Timer.h"
#include "Console.h"

namespace Chaos
{
	ImGuiProfiler::ImGuiProfiler()
	{
		Console::AddCommand("profiler", [&](){ m_showProfiler ? m_showProfiler = false : m_showProfiler = true; });
	}
	
	
	ImGuiProfiler::~ImGuiProfiler()
	{
		
	}
	
	
	void ImGuiProfiler::OnAttach()
	{
		
	}
	
	
	void ImGuiProfiler::OnDetach()
	{
		
	}
	
	
	void ImGuiProfiler::OnUpdate(float deltaTime)
	{
		
	}
	
	
	void ImGuiProfiler::OnImGuiUpdate()
	{
		if (m_showProfiler)
		{
			const float DISTANCE = 10.0f;
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			
			auto& timers = Timer::GetTimers();
			
			ImVec2 windowSize(500.0f, (25.0f * timers.size()) + 25.0f);
			
			ImVec2 windowPos = ImVec2(viewport->Pos.x + viewport->Size.x - DISTANCE, viewport->Pos.y + DISTANCE + windowSize.y);
			
			ImVec2 windowPosPivot = ImVec2(1.0f, 1.0f);
			
			ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
			ImGui::SetNextWindowSize(windowSize);
			
			ImGui::Begin("Profiler", &m_showProfiler, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
			ImGui::Text("Profiler");
			ImGui::Separator();
			
			
			for (std::pair<const char*, float> pair : timers)
			{
				ImGui::Text("%s: %f ms", pair.first, pair.second);
			}
			
			ImGui::End();
		}
	}
}