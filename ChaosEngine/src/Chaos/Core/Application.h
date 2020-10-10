#pragma once

#include "Core.h"
#include "Chaos/Core/Window.h"
#include "Chaos/Events/Event.h"
#include "Chaos/Events/ApplicationEvent.h"
#include "Chaos/Core/LayerStack.h"
#include <functional>

namespace Chaos
{
	class Renderer;
	class Camera;
	class Entity;
	class ImGuiLayer;
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
		void PopOverlay(Layer* overlay);
		
		
		inline static Application& Get() { return *s_instance; }
		inline Window& GetWindow() { return *m_window; }
		inline class Renderer& GetRenderer() { return *m_renderer; }
		inline Camera* GetMainCamera() { return m_mainCamera; }
		inline void SetMainCamera(Camera* cam) { m_mainCamera = cam; }
		inline void SetRenderingImGui(bool state) { m_renderingImGui = state; }
		inline void AddPostUpdateCallback(std::function<void()> function) {m_postUpdateSteps.push_back(function);}
		
		private:
		bool OnWindowClose(WindowCloseEvent& e);
		
		bool m_running = true;
		bool m_renderingImGui = true;
		std::unique_ptr<Window> m_window;
		std::unique_ptr<Renderer> m_renderer;
		Camera* m_mainCamera;
		Entity* m_mainCameraEntity;
		
		LayerStack m_layerStack;
		std::vector<std::function<void()>> m_postUpdateSteps;
		
		ImGuiLayer* m_guiLayer;
		
		static Application* s_instance;
	};
	
	//defined in client
	Application* CreateApplication();
}