#include "chaospch.h"
#include "Application.h"

namespace Chaos
{
	Application::Application()
	{
		mWindow = std::unique_ptr<Window>(Window::Create());
		mWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
	}

	Application::~Application()
	{

	}

	void Application::OnEvent(Event& e)
	{
		LOGCORE_TRACE(e.ToString());
	}

	void Application::Run()
	{
		while (true)
		{

		}
	}

}