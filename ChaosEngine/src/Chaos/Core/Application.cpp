#include "chaospch.h"

#include "Application.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/Input/Input.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Debug/ImGuiLayer.h"
#include "Chaos/Core/Time.h"
#include "Chaos/Core/Level.h"
#include "Chaos/Input/InputManager.h"
#include "Chaos/Debug/ImGuiConsole.h"
#include "Chaos/Debug/ImGuiEditor.h"
#include "Chaos/Debug/ImGuiDebugInfo.h"

#include <ctime>
#include <chrono>

//inspired by The Cherno's Game engine series, however has and will continue to diverge
namespace Chaos
{
	Application* Application::s_instance = nullptr;
	
	Application::Application()
	{
		//Asserting that we don't already have an instance of an application as there should only be one ever
		COREASSERT(!s_instance, "APPLICATION ALREADY EXISTS");
		s_instance = this;
		
		//Creating a window
		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		
		//Creating renderer
		m_renderer = std::unique_ptr<Renderer>(Renderer::Create());
		
		//creating input manager layer
		m_inputManager = new InputManager("./Assets/Config/Inputs.ini");
		PushLayer(m_inputManager);
		
		//Creating test overlay layer
		m_guiLayer = new ImGuiConsole();
		
		//Push test overlay layer
		PushOverlay(m_guiLayer);
		PushOverlay(new ImGuiEditor());
		PushOverlay(new ImGuiDebugInfo());
		
		//init time
		Time::Init();
		
		//starting the fixed update thread
		m_fixedUpdateThread = std::thread(&Application::FixedRun, this);
		m_fixedUpdateThread.detach();
	}
	
	Application::~Application()
	{
		
	}
	
	void Application::Run()
	{
		while (m_running)
		{
			//Update time class
			Time::m_time = m_window->GetWindowTime();
			Time::m_deltaTime = Time::m_time - Time::m_timeLastFrame;
			Time::m_timeLastFrame = Time::m_time;
			
			//NOTE: not sure why this was here
			//m_mainCamera->SetAspectRatio(m_window->GetAspectRatio());
			
			//itterate through layers
			for (Layer* layer : m_layerStack)
				layer->OnUpdate(Time::m_deltaTime);			
			
			if (m_renderingImGui)
			{
				//Currently causes black screen to be rendered over the top of the main render if not in release mode
				m_guiLayer->Begin();
				for (Layer* layer : m_layerStack)
					layer->OnImGuiUpdate();
				m_guiLayer->End();
			}
			
			//Do post update steps
			//Needed to modify the layer stack without causing itteration issues
			for (auto func : m_postUpdateSteps)
			{
				func();
			}
			//clear once finished
			m_postUpdateSteps.clear();
			
			//update the current scene after all the layers have been processed
			Level::Get()->Update(Time::m_deltaTime);
			m_window->OnUpdate();
			m_renderer->DrawFrame();
			Input::UpdateMouseEndFramePosition();
		}
	}
	
	//gets whatever scene is active and calls the fixed update function on that scene at the fixed update delta time interval
	//NOTE: should only ever be used with a seperate thread, causes the current thread to sleep
	void Application::FixedRun()
	{
		while (m_running)
		{
			std::chrono::milliseconds sleepTime(static_cast<int>(Time::GetFixedDeltaTime() * 1000));
			if (Level::Get())
			{
				Level::Get()->FixedUpdate(Time::GetFixedDeltaTime());
			}
			std::this_thread::sleep_for(sleepTime);
		}
	}
	
	void Application::OnEvent(Event& e)
	{
		//LOGCORE_TRACE(e.ToString());
		
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		
		if (e.GetEventType() == EventType::WindowResize)
		{
			m_renderer->OnWindowResized((WindowResizeEvent&)e);
		}
		
		for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
		
	}
	
	void Application::PushLayer(Layer* layer)
	{
		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}
	
	void Application::PushOverlay(Layer* overlay)
	{
		m_layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}
	
	void Application::PopOverlay(Layer* overlay)
	{
		m_layerStack.PopOverlay(overlay);
		overlay->OnDetach();
		delete overlay;
	}
	
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}
}