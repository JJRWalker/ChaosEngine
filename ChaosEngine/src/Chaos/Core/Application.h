#pragma once

#include "Core.h"
#include "Chaos/Core/Window.h"
#include "Chaos/Events/Event.h"


namespace Chaos
{
	class Application
	{
	public:

		Application();
		virtual ~Application();

		void OnEvent(Event& e);
		void Run();

		inline Window& GetWindow() { return *mWindow; }

	private:
		bool mRunning = true;
		std::unique_ptr<Window> mWindow;

	};

	//defined in client
	Application* CreateApplication();
}