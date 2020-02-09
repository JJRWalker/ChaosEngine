
#include <Chaos.h>
#include <Chaos/Events/KeyEvent.h>


class ExampleLayer : public Chaos::Layer
{
public:
	ExampleLayer()
		:Layer("Example") {}

	void OnUpdate() override
	{

	}

	void OnEvent(Chaos::Event& event) override
	{
		if (event.GetEventType() == Chaos::EventType::KeyPressed)
		{
			Chaos::KeyPressedEvent& e = (Chaos::KeyPressedEvent&)event;
			LOGINFO("{0}", (char)e.GetKeyCode());
		}
	}
};

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