#pragma once
#include "Chaos/Core/Layer.h"
#include "Vulkan/Include/vulkan/vulkan.h"
#include <ImGUI/imgui.h>

namespace Chaos
{
	class ImGuiLayer : public Layer
	{
		
		
		public:
		ImGuiLayer();
		~ImGuiLayer();
		
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override {};
		virtual void OnImGuiUpdate() override;
		
		static void Begin();
		static void End();
		
		static bool s_ImGuiInited;
	};
}