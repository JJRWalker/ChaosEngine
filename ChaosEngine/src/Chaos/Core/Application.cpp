#include "chaospch.h"
#include "Application.h"
#include <ctime>
#include <chrono>
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/Input/Input.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Entity/Components/Camera.h"
#include "Chaos/Debug/ImGuiLayer.h"
#include "GLFW/glfw3.h"

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
		//Creating a default camera
		m_mainCamera = new Camera();
		//Creating renderer
		m_renderer = std::unique_ptr<Renderer>(Renderer::Create());
		//Creating test overlay layer
		m_guiLayer = new ImGuiLayer();
		//Push test overlay layer
		PushOverlay(m_guiLayer);
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_running)
		{
			float time = (float)glfwGetTime();
			m_deltaTime = time - m_timeLastFrame;
			m_timeLastFrame = time;

			for (Layer* layer : m_layerStack)
				layer->OnUpdate(m_deltaTime);			


			m_window->OnUpdate();

			m_renderer->DrawFrame();

			//Currently causes black screen to be rendered over the top of the main render, need to change how the pipeline and descriptor sets are handled by Vulkan/ImGui
			m_guiLayer->Begin();
			for (Layer* layer : m_layerStack)
				layer->OnImGuiRender();
			
			m_guiLayer->End();

			//LOGCORE_INFO("Time to renderframe: {0} FPS: {1}", mDeltaTime, 1 / mDeltaTime);
		}
	}

	void Application::OnEvent(Event& e)
	{
		//LOGCORE_TRACE(e.ToString());

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		if (e.GetEventType() == EventType::WindowResize)
		{
			m_renderer->WindowResized();
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
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}
}