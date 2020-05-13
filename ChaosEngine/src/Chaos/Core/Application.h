#pragma once

#include "Core.h"
#include "Chaos/Core/Window.h"
#include "Chaos/Events/Event.h"
#include "Chaos/Events/ApplicationEvent.h"
#include "Chaos/Core/LayerStack.h"
#include "Chaos/Components/Camera.h"

namespace Chaos
{

	class Application
	{
	public:

		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);	

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
	

		inline static Application& Get() { return *sInstance; }
		inline Window& GetWindow() { return *mWindow; }
		inline class Renderer& GetRenderer() { return *mRenderer; }
		inline class Camera& GetCamera() { return *mMainCamera; }
		inline void SetMainCamera(Camera& camera) { mMainCamera = &camera; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		bool mRunning = true;
		std::unique_ptr<Window> mWindow;
		std::unique_ptr<class Renderer> mRenderer;
		Camera* mMainCamera;

		LayerStack mLayerStack;

		float mDeltaTime = 0.0f;
		float mTimeLastFrame = 0.0f;

		class ImGuiLayer* guiLayer;

		static Application* sInstance;
	};

	//defined in client
	Application* CreateApplication();
}