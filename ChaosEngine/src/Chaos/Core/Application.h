#pragma once

#include "Core.h"
#include "Chaos/Core/Window.h"
#include "Chaos/Events/Event.h"
#include "Chaos/Events/ApplicationEvent.h"
#include "Chaos/Core/LayerStack.h"
//#include "Chaos/Renderer/Renderer.h"


namespace Chaos
{
	class Application
	{
	public:

		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);	

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
	

		inline static Application& Get() { return *sInstance; }
		inline Window& GetWindow() { return *mWindow; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		bool mRunning = true;
		std::unique_ptr<Window> mWindow;
		//std::unique_ptr<Renderer> mRenderer;

		//std::unique_ptr<Renderer> mRenderer;

		LayerStack mLayerStack;


		static Application* sInstance;
	};

	//defined in client
	Application* CreateApplication();
}