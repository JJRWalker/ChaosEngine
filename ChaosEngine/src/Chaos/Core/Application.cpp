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
#include "Chaos/Debug/ImGuiProfiler.h"
#include "Chaos/Nodes/Camera.h"

#include <ctime>
#include <chrono>

#include "Chaos/Debug/Timer.h"

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
		m_renderer = std::unique_ptr<Renderer>(Renderer::Create(m_window.get()));
		m_renderer->Init();
		m_renderer->InitImgui();
		
		//creating input manager layer
		m_inputManager = new InputManager("./Assets/Config/Inputs.ini");
		PushLayer(m_inputManager);;
		
		//Push test overlay layer
		PushOverlay(new ImGuiConsole());
		PushOverlay(new ImGuiEditor());
		PushOverlay(new ImGuiDebugInfo());
		PushOverlay(new ImGuiProfiler());
		
		//init time
		Time::Init();		
	}
	
	Application::~Application()
	{
		
	}
	
	void Application::Run()
	{
		// NOTE: seemed to cause issues at lower framerates. for now single threaded updates
		//StartFixedUpdateThread();
		while (m_running)
		{
			PROFILED_FUNC();
			
			// Update time class
			Time::m_time = m_window->GetWindowTime();
			Time::m_unscaledDeltaTime = (float)(Time::m_time - Time::m_timeLastFrame);
			Time::m_deltaTime = Time::m_unscaledDeltaTime * Time::m_timeScale;
			Time::m_timeSinceLastFixedUpdate += Time::m_deltaTime;
			Time::m_timeLastFrame = Time::m_time;
			
			Level* level = Level::Get();
			
			if (Time::m_timeSinceLastFixedUpdate >= Time::m_fixedDeltaTime)
			{
				float stepsToSimulate = Time::m_timeSinceLastFixedUpdate / Time::m_fixedDeltaTime;
				for(int i = 0; i < (int)stepsToSimulate; ++i)
				{
					for (Layer* layer : m_layerStack)
					{
						layer->OnFixedUpdate(Time::m_fixedDeltaTime);
					}
					
					
					if (level)
					{
						level->OnFixedUpdate(Time::m_fixedDeltaTime);
					}
				}
				
				Time::m_timeSinceLastFixedUpdate = (stepsToSimulate - (int)stepsToSimulate) * Time::m_fixedDeltaTime;
			}
			
			//NOTE: this should be done when changing the resolution
			//m_mainCamera->SetAspectRatio(m_window->GetAspectRatio());
			
			
			// itterate through layers
			for (Layer* layer : m_layerStack)
				layer->OnUpdate(Time::m_deltaTime);
			
			if (m_renderingImGui)
			{
				ImGuiLayer::Begin();
				for (Layer* layer : m_layerStack)
					layer->OnImGuiUpdate();
				ImGuiLayer::End();
			}
			
			// update the current scene after all the layers have been processed
			if (level)
				level->OnUpdate(Time::m_deltaTime);
			
			m_window->OnUpdate();
			
			m_renderer->DrawFrame();
			
			
			// Do post update steps
			//Needed to modify the layer stack without causing itteration issues
			for (auto func : m_postUpdateSteps)
			{
				func();
			}
			// clear once finished
			m_postUpdateSteps.clear();
			
			
			// NOTE: here lies some rudementary garbage collection. Not a huge fan of anything more complex than this
			// Needed as deleting a node mid way through an update loop isn't the best idea
			// if the level needs to be destroyed, skip the rest of the loop
			if (level->PendingDestroy)
			{
				level->OnEnd();
				delete Level::Get();
				continue;
			}
			
			// delete any nodes pending destruction
			for (int node = 0; node < level->NodeCount; ++node)
			{
				if (level->Nodes[node][0]->PendingDestruction)
				{
					delete level->Nodes[node][0];
					--node;
				}
				else
				{
					for (int child = 1; child < level->Nodes[node][0]->ChildCount; ++child)
					{
						if (level->Nodes[node][child]->PendingDestruction)
						{
							delete level->Nodes[node][child];
							--child;
						}
					}
				}
			}
			
			Input::UpdateMouseEndFramePosition();
		}
		
		//while(m_runningFixedUpdate); // wait for last fixed update to finish
	}
	
	//gets whatever level is active and calls the fixed update function on that level at the fixed update delta time interval
	//NOTE: should only ever be used with a seperate thread, causes the current thread to sleep
	void Application::FixedRun()
	{
		while (m_running)
		{
			m_runningFixedUpdate = true;
			float fixedDelta = Time::GetFixedDeltaTime();
			
			std::chrono::milliseconds sleepTime(static_cast<int>(fixedDelta * 1000));
			
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
			
			m_runningFixedUpdate = false;
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