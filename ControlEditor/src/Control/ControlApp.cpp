#include <Chaos.h>
#include <Chaos/Core/EntryPoint.h>
#include <Control/Editor/EditorLayer.h>

namespace Chaos
{
	class Control : public Application
	{
	public:
		Control()
		{
			PushOverlay(new EditorLayer());
			Application::Get().SetRenderingImGui(true);
		}
		~Control()
		{

		}
	};

	Application* CreateApplication()
	{
		return new Control();
		#define CHAOS_PROFILE
	}
}

