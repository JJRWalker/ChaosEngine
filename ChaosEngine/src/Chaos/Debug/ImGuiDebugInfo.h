/* date = October 10th 2020 9:13 am */

#ifndef _IM_GUI_DEBUG_INFO_H
#define _IM_GUI_DEBUG_INFO_H
#include <Chaos/Debug/ImGuiLayer.h>

namespace Chaos
{
	class ImGuiDebugInfo : public ImGuiLayer
	{
		public:
		ImGuiDebugInfo();
		
		virtual void OnUpdate(float deltaTime) override;
		virtual void OnImGuiUpdate() override;
		
		private:
		bool m_showDebug = false;
		int m_fps = 0;
	};
	
}
#endif //_IM_GUI_DEBUG_INFO_H