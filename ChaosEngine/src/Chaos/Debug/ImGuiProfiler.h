/* date = May 10th 2021 9:31 pm */

#ifndef _IM_GUI_PROFILER_H
#define _IM_GUI_PROFILER_H

#include "ImGuiLayer.h"

namespace Chaos
{
	class ImGuiProfiler : public ImGuiLayer
	{
		public:
		ImGuiProfiler();
		~ImGuiProfiler();
		
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnImGuiUpdate() override;
		
		private:
		bool m_showProfiler = false;
	};
}

#endif //_IM_GUI_PROFILER_H
