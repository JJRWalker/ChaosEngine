
#include <Chaos.h>
#include <Chaos/Events/KeyEvent.h>


class ExampleLayer : public Chaos::Layer
{
public:
	ExampleLayer()
		:Layer("Example") {}

	void OnUpdate() override
	{
		if (Chaos::Input::IsKeyPressed(KEY_W))
		{
			LOGINFO("W key pressed");
		}

		if (Chaos::Input::IsKeyPressed(KEY_A))
		{
			LOGINFO("A key pressed");
		}

		if (Chaos::Input::IsKeyPressed(KEY_S))
		{
			LOGINFO("S key pressed");
		}

		if (Chaos::Input::IsKeyPressed(KEY_D))
		{
			LOGINFO("D key pressed");
		}
	}

	void OnEvent(Chaos::Event& event) override
	{
		if (event.GetEventType() == Chaos::EventType::KeyPressed)
		{

			//Chaos::KeyPressedEvent& e = (Chaos::KeyPressedEvent&)event;
			//LOGINFO("{0}", (char)e.GetKeyCode());
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