#include "chaospch.h"

#include "Application.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/Input/Input.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Renderer/Material.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Debug/ImGuiLayer.h"
#include "Chaos/Core/Time.h"
#include "Chaos/Core/Level.h"
#include "Chaos/Input/InputManager.h"
#include "Chaos/Debug/ImGuiConsole.h"
#include "Chaos/Debug/ImGuiEditor.h"
#include "Chaos/Debug/ImGuiDebugInfo.h"
#include "Chaos/Nodes/Camera.h"

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
		
		m_mainCamera = Level::Get()->MainCamera();
		
		//Creating renderer
		m_renderer = std::unique_ptr<Renderer>(Renderer::Create(m_window.get()));
		m_renderer->InitImgui();
		m_renderer->SetCamera(m_mainCamera);
		
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
	}
	
	Application::~Application()
	{
		
	}
	
	void Application::Run()
	{
		StartFixedUpdateThread();
		while (m_running)
		{
			//Update time class
			Time::m_time = m_window->GetWindowTime();
			Time::m_deltaTime = (float)(Time::m_time - Time::m_timeLastFrame);
			Time::m_timeLastFrame = Time::m_time;
			
			//m_mainCamera->Translate(Vec2(Input::GetButton("horizontal"), Input::GetButton("vertical")) * Time::m_deltaTime * 10);
			
			//NOTE: this should be done when changing the resolution
			//m_mainCamera->SetAspectRatio(m_window->GetAspectRatio());
			
			//itterate through layers
			for (Layer* layer : m_layerStack)
				layer->OnUpdate(Time::m_deltaTime);			
			
			if (m_renderingImGui)
			{
				//Currently causes black screen to be rendered over the top of the main render if not in release mode
				ImGuiLayer::Begin();
				for (Layer* layer : m_layerStack)
					layer->OnImGuiUpdate();
				ImGuiLayer::End();
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
			Level::Get()->OnUpdate(Time::m_deltaTime);
			m_window->OnUpdate();
			m_renderer->DrawFrame();
			Input::UpdateMouseEndFramePosition();
		}
	}
	
	//gets whatever level is active and calls the fixed update function on that level at the fixed update delta time interval
	//NOTE: should only ever be used with a seperate thread, causes the current thread to sleep
	void Application::FixedRun()
	{
		while (m_running)
		{
			float fixedDelta = Time::GetFixedDeltaTime();
			
			//NOTE: not sure why but this needs to be multiplied by 500 instead of 1000
			std::chrono::milliseconds sleepTime(static_cast<int>(fixedDelta * 500));
			
			if (m_pauseFixedUpdate)
			{
				std::this_thread::sleep_for(sleepTime);
				continue;
			}
			
			for (Layer* layer : m_layerStack)
			{
				layer->OnFixedUpdate(fixedDelta);
			}
			
			
			if (Level::Get())
			{
				Level::Get()->OnFixedUpdate(fixedDelta);
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
	
	void Application::StartFixedUpdateThread()
	{
		m_fixedUpdateThread = std::thread(&Application::FixedRun, this);
		m_fixedUpdateThread.detach();
	}
	
	void Application::PauseFixedUpdateThread()
	{
		m_pauseFixedUpdate = true;
	}
	
	void Application::ResumeFixedUpdateThread()
	{
		m_pauseFixedUpdate = false;
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