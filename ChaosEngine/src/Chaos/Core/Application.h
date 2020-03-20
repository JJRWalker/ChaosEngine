#pragma once

#include "Core.h"
#include "Chaos/Core/Window.h"
#include "Chaos/Events/Event.h"
#include "Chaos/Events/ApplicationEvent.h"
#include "Chaos/Core/LayerStack.h"


namespace Chaos
{
	extern class Renderer;
	extern class ImGuiLayer;

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
		inline Renderer& GetRenderer() { return *mRenderer; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		bool mRunning = true;
		std::unique_ptr<Window> mWindow;
		std::unique_ptr<Renderer> mRenderer;

		LayerStack mLayerStack;

		float mDeltaTime = 0.0f;
		float mTimeLastFrame = 0.0f;

		ImGuiLayer* guiLayer;

		static Application* sInstance;
	};

	//defined in client
	Application* CreateApplication();
}