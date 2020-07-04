#pragma once

#include "Core.h"
#include "Chaos/Core/Window.h"
#include "Chaos/Events/Event.h"
#include "Chaos/Events/ApplicationEvent.h"
#include "Chaos/Core/LayerStack.h"

namespace Chaos
{
	class Renderer;
	class Camera;
	class Application
	{
	public:

		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);	
		void Close() { m_running = false; }

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
	

		inline static Application& Get() { return *s_instance; }
		inline Window& GetWindow() { return *m_window; }
		inline class Renderer& GetRenderer() { return *m_renderer; }
		inline Camera* GetMainCamera() { return m_mainCamera; }
		inline void SetMainCamera(Camera* cam) { m_mainCamera = cam; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		bool m_running = true;
		std::unique_ptr<Window> m_window;
		std::unique_ptr<Renderer> m_renderer;
		Camera* m_mainCamera;

		LayerStack m_layerStack;

		float m_deltaTime = 0.0f;
		float m_timeLastFrame = 0.0f;

		class ImGuiLayer* m_guiLayer;

		static Application* s_instance;
	};

	//defined in client
	Application* CreateApplication();
}