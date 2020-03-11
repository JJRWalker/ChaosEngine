#include "chaospch.h"
#include "Application.h"
#include <ctime>
#include <chrono>
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/Input/Input.h"

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
	
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{

		Vec2* pos = new Vec2(0.f, 0.f);
		int xModifier = 1;
		int yModifier = 1;
		while (mRunning)
		{
			for (Layer* layer : mLayerStack)
				layer->OnUpdate();

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
			
				//mRenderer->DrawQuad(pos, Vec2(0.f, 0.f), "../Game/textures/test.png");
				mRenderer->DrawQuad(pos, new Vec2(5.f, 5.f), "../Game/textures/sprite-test.png");

				pos->Y += 0.01f * yModifier;
				pos->X += 0.01f * xModifier;
				//mRenderer->DrawFrame();
				/*
				//PERFORMANCE TESTING
				auto begin = std::chrono::high_resolution_clock::now();
				int drawCalls = 10000;
				int sampleSize = 10;
				float total = 0;
				for (int x = 0; x < sampleSize; ++x)
				{
					for (int i = 0; i < drawCalls; ++i)
					{
						mRenderer->DrawFrame();
					}
					auto end = std::chrono::high_resolution_clock::now();
					auto dur = end - begin;
					auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
					total += ms.count();
				}
				total /= sampleSize;
				LOGCORE_WARN("Average time taken to render {0} frames: {1}", drawCalls, total);
				LOGCORE_WARN("Sample size: {0}", sampleSize);
				break;
				*/
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