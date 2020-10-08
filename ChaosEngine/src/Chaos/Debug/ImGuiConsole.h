/* date = October 7th 2020 7:19 pm */

#ifndef _IM_GUI_CONSOLE_H
#define _IM_GUI_CONSOLE_H

#include <Chaos/Debug/ImGuiLayer.h>
#include <Chaos/Debug/Console.h>

namespace Chaos
{
	class ImGuiConsole : public ImGuiLayer
	{
		public:
		ImGuiConsole();
		
		virtual void OnImGuiUpdate() override;
		
		private:
		static int ConsoleEditTextCallbackStub(ImGuiInputTextCallbackData* data);
		int ConsoleTextEditCallback(ImGuiInputTextCallbackData* data);
		
		private:
		bool m_showConsole = false;
		bool m_consoleKeyReleased = true;
		bool m_autoScroll = true;
		bool m_resetFocus = true;
		
		char m_inputBuffer[256];
	};
}

#endif //_IM_GUI_CONSOLE_H
