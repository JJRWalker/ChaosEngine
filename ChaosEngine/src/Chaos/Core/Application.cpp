#include "chaospch.h"
#include "Application.h"

#include "Chaos/Renderer/Renderer.h"

namespace Chaos
{
	Application* Application::sInstance = nullptr;

	Application::Application()
	{
		//Asserting that we don't already have an instance of an application as there should only be one ever
		COREASSERT(!sInstance, "APPLICATION ALREADY EXISTS");
		sInstance = this;

		//Creating a window
		mWindow = std::unique_ptr<Window>(Window::Create());
		mWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer* vulkan = new Renderer();
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (mRunning)
		{
			for (Layer* layer : mLayerStack)
				layer->OnUpdate();

			mWindow->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		//LOGCORE_TRACE(e.ToString());

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = mLayerStack.end(); it != mLayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}

	}

	void Application::PushLayer(Layer* layer)
	{
		mLayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		mLayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}
}