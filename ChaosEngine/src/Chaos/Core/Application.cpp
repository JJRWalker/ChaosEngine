#include "chaospch.h"
#include "Application.h"
#include <ctime>
#include <chrono>
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/Input/Input.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Debug/ImGuiLayer.h"

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

		mLayerStack.PushLayer(guiLayer);
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		Texture* testSprite = Texture::Create("../Game/textures/sprite-test.png", 1);
		Texture* test = Texture::Create("../Game/textures/Floor.jpg", 20);
		Texture* blank = Texture::Create("", 1);
		Vec2* pos = new Vec2(0.f, 0.f);
		int xModifier = 1;
		int yModifier = 1;
		while (mRunning)
		{
			//PERFORMANCE TESTING
			auto begin = std::chrono::high_resolution_clock::now();
			int drawCalls = 10000;
			int sampleSize = 10;
			float total = 0;

			for (Layer* layer : mLayerStack)
			{
				layer->OnUpdate();
			}

			guiLayer->Begin();
			for (Layer* layer : mLayerStack)
			{
				layer->OnImGuiRender();
			}
			guiLayer->End();

			mWindow->OnUpdate();
			
			if (mWindow->GetWidth() != 0 && mWindow->GetHeight() != 0)
			{
				if (Input::IsKeyPressed(KEY_A))
				{
					xModifier = -1;
				}
				else if (Input::IsKeyPressed(KEY_D))
				{
					xModifier = 1;
				}
				else
				{
					xModifier = 0;
				}

				if (Input::IsKeyPressed(KEY_S))
				{
					yModifier = -1;
				}
				else if (Input::IsKeyPressed(KEY_W))
				{
					yModifier = 1;
				}
				else
				{
					yModifier = 0;
				}
				pos->Y += 0.01f * yModifier;
				pos->X += 0.01f * xModifier;

				mRenderer->DrawQuad(new Vec2(0.f, 0.f), new Vec2(20.f, 20.f), test);
				mRenderer->DrawQuad(pos, new Vec2(1.f, 1.f), testSprite);

				mRenderer->DrawFrame();

				auto end = std::chrono::high_resolution_clock::now();
				auto dur = end - begin;
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
				//LOGCORE_WARN("FPS: {0}", ( 1000.f / ms.count()));

			}				
		}
		//mRenderer->WaitIdle();
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