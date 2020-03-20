#include "chaospch.h"
#include "Application.h"
#include <ctime>
#include <chrono>
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/Input/Input.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Debug/ImGuiLayer.h"
#include "GLFW/glfw3.h"

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

		mRenderer = std::unique_ptr<Renderer>(Renderer::Create());

		guiLayer = new ImGuiLayer();

		PushOverlay(guiLayer);
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (mRunning)
		{
			float time = (float)glfwGetTime();
			mDeltaTime = time - mTimeLastFrame;
			mTimeLastFrame = time;

			for (Layer* layer : mLayerStack)			
				layer->OnUpdate(mDeltaTime);
			

			guiLayer->Begin();
			for (Layer* layer : mLayerStack)
				layer->OnImGuiRender();
			
			guiLayer->End();

			mWindow->OnUpdate();
			
			if (mWindow->GetWidth() != 0 && mWindow->GetHeight() != 0)
			{
				mRenderer->DrawFrame();
			}				
		}
		mRenderer->WaitIdle();
	}

	void Application::OnEvent(Event& e)
	{
		//LOGCORE_TRACE(e.ToString());

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		if (e.GetEventType() == EventType::WindowResize)
		{
			mRenderer->WindowResized();
		}

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