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
		virtual void OnFixedUpdate(float fixedDelta) override;
		virtual void OnImGuiUpdate() override;
		
		private:
		bool m_showDebug = true;
		int m_fps = 0;
		float m_averageFPS = 0;
		int m_framesSinceAvgFrameCheck = 0;
		float m_timeSinceAvgFrameCheck = 0;
		float m_AvgFrameCheckTime = 1.0f;
		int m_fixedFPS = 0;
	};
	
}
#endif //_IM_GUI_DEBUG_INFO_H