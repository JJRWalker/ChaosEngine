#pragma once
#include <Chaos.h>
#include <Chaos/Core/EntryPoint.h>

//Template for a fresh project

class ExampleLayer : public Chaos::Layer
{
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
class GameApp : public Chaos::Application
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

Chaos::Application* Chaos::CreateApplication()
{
	return new GameApp();
}