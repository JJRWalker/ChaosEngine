#include <chaospch.h>
#include <Chaos/Debug/ImGuiFileExplorer.h>
#include <Chaos/Core/Application.h>

namespace Chaos
{
	ImGuiFileExplorer::ImGuiFileExplorer(std::string& filePath, std::function<void()> callback) : m_filePath(filePath), m_callback(callback)
	{
		memset(m_tempPath, 0, sizeof(m_tempPath));
		m_rootPath = std::filesystem::current_path();
		strcpy(m_tempPath, "Assets");
	}
	void ImGuiFileExplorer::OnDetach()
	{
		
	}
	
	void ImGuiFileExplorer::OnImGuiUpdate()
	{
		ImGui::SetNextWindowSize(ImVec2(500, 300));
		ImGui::Begin("File Explorer");
		if (ImGui::Button("^"))
		{
			strcpy(m_tempPath, std::filesystem::proximate(m_tempPath, m_rootPath).parent_path().string().c_str());
			LOGCORE_TRACE("{0}",std::filesystem::proximate(m_tempPath, m_rootPath).parent_path().string().c_str());
		}
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -30), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (std::filesystem::exists(std::filesystem::proximate(m_tempPath, m_rootPath)) )
		{
			int i = 0;
			static int selected = 0;
			if (std::filesystem::proximate(m_tempPath, m_rootPath).extension() == "")
			{
				for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::proximate(m_tempPath, m_rootPath)))
				{
					i++;
					if (ImGui::Selectable(entry.path().string().c_str(), selected == i))
					{
						LOGCORE_TRACE("{0}", std::filesystem::relative(entry.path(), m_rootPath).string());
						if (strlen(std::filesystem::relative(entry.path(), m_rootPath).string().c_str()) < 256)
						{
							strcpy(m_tempPath, std::filesystem::relative(entry.path(), m_rootPath).string().c_str());
						}
						selected = i;
					}
				}
				
			}
			else
			{
				for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::proximate(m_tempPath, m_rootPath).parent_path()))
				{
					i++;
					if (ImGui::Selectable(entry.path().string().c_str(), selected == i))
					{
						LOGCORE_TRACE("{0}", std::filesystem::relative(entry.path(), m_rootPath).string());
						if (strlen(std::filesystem::relative(entry.path(), m_rootPath).string().c_str()) < 256)
						{
							strcpy(m_tempPath, std::filesystem::relative(entry.path(), m_rootPath).string().c_str());
						}
						selected = i;
					}
				}
			}
		}
		
		ImGui::EndChild();
		ImGui::Separator();
		
		if (ImGui::InputText("Input", m_tempPath, 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			m_filePath = m_tempPath;
		}
		ImGui::SameLine();
		if (ImGui::Button("Open"))
		{
			m_filePath = m_tempPath;
			m_callback();
			Application::Get().PopOverlay(this);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			Application::Get().PopOverlay(this);
		}
		//memcpy(m_filePath, "Hello there", sizeof("Hello there") + 1);
		ImGui::End();
	}
}