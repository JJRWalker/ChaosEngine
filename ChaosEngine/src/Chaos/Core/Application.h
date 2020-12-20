#pragma once

#include "Core.h"
#include "Chaos/Core/Window.h"
#include "Chaos/Events/Event.h"
#include "Chaos/Events/ApplicationEvent.h"
#include "Chaos/Core/LayerStack.h"
#include <functional>
#include <thread>

namespace Chaos
{
	class Renderer;
	class Camera;
	class Entity;
	class ImGuiLayer;
	class InputManager;
	class Application
	{
		public:
		
		Application();
		virtual ~Application();
		
		//main funcs
		void Run();
		void OnEvent(Event& e);	
		void Close() { m_running = false; }
		
		//layer stack interaction
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);
		
		//Getters
		inline static Application& Get() { return *s_instance; }
		inline Window& GetWindow() { return *m_window; }
		inline class Renderer& GetRenderer() { return *m_renderer; }
		inline Camera* GetMainCamera() { return m_mainCamera; }
		
		//setters
		inline void SetMainCamera(Camera* cam) { m_mainCamera = cam; }
		inline void SetRenderingImGui(bool state) { m_renderingImGui = state; }
		
		//post update callback
		inline void AddPostUpdateCallback(std::function<void()> function) {m_postUpdateSteps.push_back(function);}
		
		//private funcs
		private:
		bool OnWindowClose(WindowCloseEvent& e);
		
		//to be ran on seperate threads
		void FixedRun();
		
		//private vars
		private:
		//App owned objects
		bool m_running = true;
		bool m_renderingImGui = true;
		std::unique_ptr<Window> m_window;
		std::unique_ptr<Renderer> m_renderer;
		Camera* m_mainCamera;
		Entity* m_mainCameraEntity;
		
		//threads
		std::thread m_fixedUpdateThread;
		
		//layer stack
		LayerStack m_layerStack;
		std::vector<std::function<void()>> m_postUpdateSteps;
		
		//debug layers
		ImGuiLayer* m_guiLayer;
		InputManager* m_inputManager;
		
		
		static Application* s_instance;
	};
	
	//defined in client
	Application* CreateApplication();
}