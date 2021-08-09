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
		void Close();
		void Play();
		void EndPlay();
		
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);
		
		static Application& Get();
		Window& GetWindow();
		Renderer& GetRenderer();
		
		//post update callback
		void AddPostUpdateCallback(std::function<void()> function);
		
		private:
		bool OnWindowClose(WindowCloseEvent& e);
		
		public:
		bool RenderImGui = true;
		
		private:
		bool m_running = true;
		bool m_playing = true;
		
		std::unique_ptr<Window> m_window;
		std::unique_ptr<Renderer> m_renderer;
		
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