#include <chaospch.h>
#include <Chaos/Debug/ImGuiConsole.h>
#include <Chaos/Input/Input.h>

namespace Chaos
{
	std::unordered_map<std::string, std::function<void()>> Console::Commands;
	std::vector<char*> Console::ConsoleHistory;
	
	ImGuiConsole::ImGuiConsole()
	{
		memset(m_inputBuffer, 0, sizeof(m_inputBuffer));
		Console::AddCommand("clr", &Console::ClearHistory);
	}
	
	void ImGuiConsole::OnImGuiUpdate()
	{
		if (Input::IsKeyPressed(KEY_GRAVE_ACCENT) && m_consoleKeyReleased)
		{
			m_consoleKeyReleased = false;
			
			m_showConsole ? m_showConsole = false : m_showConsole = true;
			
			m_resetFocus = true;
		}
		else if(Input::IsKeyReleased(KEY_GRAVE_ACCENT))
		{
			m_consoleKeyReleased = true;
		}
		
		if (m_showConsole)
		{
			
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;
			
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.1f));
			ImGui::SetNextWindowPos(ImVec2(0,0));
			ImGui::SetNextWindowSize(ImVec2(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() / 3));
			ImGui::Begin("Console", NULL, window_flags);
			ImGui::PopStyleColor();
			
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -30), false, ImGuiWindowFlags_HorizontalScrollbar);
			for (auto log : Console::ConsoleHistory)
			{
				ImGui::TextWrapped(log);
			}
			
			if (m_autoScroll)
				ImGui::SetScrollHere(1.0f);
			
			ImGui::EndChild();
			ImGui::Separator();
			
			if (ImGui::InputText("Input", m_inputBuffer, 256, ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, &ConsoleEditTextCallbackStub, (void*)this))
			{
				Console::ParseCommand(m_inputBuffer);
				strcpy(m_inputBuffer, "");
				m_resetFocus = true;
			}
			
			if (m_resetFocus)
			{
				ImGui::SetKeyboardFocusHere(-1);
				m_resetFocus = false;
			}
			
			ImGui::SameLine();
			ImGui::Checkbox("Auto-scroll", &m_autoScroll); ImGui::SameLine();
			if (ImGui::SmallButton("Clear")) 
			{ 
				Console::ClearHistory();
			}
			
			ImGui::End();
		}
		
	}
	
	//Currently just returns 0, text input reuqired a callback function but we don't really need that complex a system
	int ImGuiConsole::ConsoleEditTextCallbackStub(ImGuiInputTextCallbackData* data)
	{
		//ImGuiConsole* console = (ImGuiConsole*)data->UserData;
		return 0;
	}
	
	int ImGuiConsole::ConsoleTextEditCallback(ImGuiInputTextCallbackData* data)
	{
		return 0;
	}
	
	
}