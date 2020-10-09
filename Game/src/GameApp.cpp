#ifndef _GAME_APP_DEFINED
#define _GAME_APP_DEFINED
#include <Chaos.h>
#include <Chaos/Core/EntryPoint.h>
#include <Chaos/Entity/Components/Camera.h>
#include <Chaos/Entity/Components/Render.h>
#include <Chaos/Entity/Components/BoxCollider2D.h>
//Template for a fresh project


namespace Chaos
{
	class ExampleLayer : public Layer
	{
		Entity entity;
		
		public:
		ExampleLayer()
			:Layer("Example") {};
		
		
		void OnUpdate(float deltaTime) override
		{
			//called every frame
		}
		
		void OnAttach() override
		{
			//called when this layer is attached to the layer stack
			entity.AddComponent<Render>();
			entity.AddComponent<BoxCollider2D>();
		}
		
		void OnDetach() override
		{
			//called when removed from layer stack, use to free resources
		}
		
		void OnEvent(Chaos::Event& event) override
		{
			//On every window event
		}
	};
	
	//entry point for the application
	class GameApp : public Application
	{
		public:
		GameApp()
		{
			PushLayer(new ExampleLayer());
		}
		
		~GameApp()
		{
			
		}
	};
	
	Application* Chaos::CreateApplication()
	{
		return new GameApp();
	}
}
#endif