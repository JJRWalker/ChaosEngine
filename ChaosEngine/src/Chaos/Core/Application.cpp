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
			
			// Perform fixed update steps
			// NOTE: would do this on a seperate thread but that caused issues at lower frame rates for some reason. So here it lies
			if (Time::m_timeSinceLastFixedUpdate >= Time::m_fixedDeltaTime)
			{
				float stepsToSimulate = Time::m_timeSinceLastFixedUpdate / Time::m_fixedDeltaTime;
				
				for(int i = 0; i < stepsToSimulate; ++i)
				{
					for (Layer* layer : m_layerStack)
					{
						layer->OnFixedUpdate(Time::m_fixedDeltaTime);
					}
					
					
					if (level && m_playing)
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
			
			if (RenderImGui)
			{
				ImGuiLayer::Begin();
				for (Layer* layer : m_layerStack)
					layer->OnImGuiUpdate();
				ImGuiLayer::End();
			}
			
			// update the current scene after all the layers have been processed
			if (level)
			{
				if (m_playing)
					level->OnUpdate(Time::m_deltaTime);
				else
					level->OnEditorUpdate(Time::m_unscaledDeltaTime);
			}
			
			m_window->OnUpdate();
			
			m_renderer->DrawFrame();
			
			// NOTE: here lies some rudementary garbage collection. Not a huge fan of anything more complex than this
			// Needed as deleting a node mid way through an update loop isn't the best idea
			// delete any nodes pending destruction
			for (int node = 0; node < level->NodeCount; ++node)
			{
				if (level->Nodes[node]->PendingDestruction)
				{
					delete level->Nodes[node];
					--node;
				}
			}
			
			Input::UpdateMouseEndFramePosition();
			
			// Do post update steps
			//Needed to modify the layer stack without causing itteration issues
			for (auto func : m_postUpdateSteps)
			{
				func();
			}
			// clear once finished
			m_postUpdateSteps.clear();
		}
	}
	
	
	void Application::Play()
	{
		Level::Save("level-defaultstate-save.lvl");
		Level::Get()->OnStart();
		
		m_playing = true;
	}
	
	
	void Application::EndPlay()
	{
		Level::Get()->OnEnd();
		m_playing = false;
		
		Level::Load("level-defaultstate-save.lvl");
	}
	
	
	
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		
		if (e.GetEventType() == EventType::WindowResize)
		{
			m_renderer->OnWindowResized((WindowResizeEvent&)e);
			Level::Get()->MainCamera->SetAspectRatio(m_window->GetAspectRatio());
		}
		
		for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
		
	}
	
	
	void Application::Close()
	{
		m_running = false; 
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
	
	
	Application& Application::Get() 
	{
		return *s_instance; 
	}
	
	
	Window& Application::GetWindow() 
	{
		return *m_window;
	}
	
	
	Renderer& Application::GetRenderer() 
	{
		return *m_renderer;
	}
	
	
	void Application::AddPostUpdateCallback(std::function<void()> function) 
	{
		m_postUpdateSteps.push_back(function);
	}
	
	
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}
}