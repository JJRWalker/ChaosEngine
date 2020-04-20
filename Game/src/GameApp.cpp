
#include <Chaos.h>
#include <Chaos/Events/KeyEvent.h>


class ExampleLayer : public Chaos::Layer
{
public:
	ExampleLayer()
		:Layer("Example") {}

	float moveSpeed = 1.f;
	int xDir = 0;
	int yDir = 0;
	Chaos::Texture* player = Chaos::Texture::Create("../Game/textures/sprite-test.png", 1);
	Chaos::Texture* floor = Chaos::Texture::Create("../Game/textures/Floor.jpg", 20);
	Chaos::Texture* test = Chaos::Texture::Create("../Game/textures/test.png", 1);
	Chaos::Texture* big = Chaos::Texture::Create("../Game/textures/big-texture.png", 1);
	Chaos::Texture* test2 = Chaos::Texture::Create("../Game/textures/test2.png", 1);
	float x = 0;
	float y = 0;

	void OnUpdate(float deltaTime) override
	{
		if (Chaos::Input::IsKeyPressed(KEY_W))
		{
			yDir = 1;
		}
		else if (Chaos::Input::IsKeyPressed(KEY_S))
		{
			yDir = -1;
		}
		else
		{
			yDir = 0;
		}

		if (Chaos::Input::IsKeyPressed(KEY_A))
		{
			xDir = -1;
		}
		else if (Chaos::Input::IsKeyPressed(KEY_D))
		{
			xDir = 1;
		}
		else
		{
			xDir = 0;
		}
		x += xDir * (moveSpeed * deltaTime);
		y += yDir * (moveSpeed * deltaTime);

		Chaos::Application::Get().GetRenderer().DrawQuad(new Chaos::Vec2(0.f, 0.f), new Chaos::Vec2(20.f, 20.f), floor);
		Chaos::Application::Get().GetRenderer().DrawQuad(new Chaos::Vec2(4.f, 0.f), new Chaos::Vec2(1.f, 1.f), test);
		Chaos::Application::Get().GetRenderer().DrawQuad(new Chaos::Vec2(3.f, 0.f), new Chaos::Vec2(1.f, 1.f), test);
		Chaos::Application::Get().GetRenderer().DrawQuad(new Chaos::Vec2(2.f, 0.f), new Chaos::Vec2(1.f, 1.f), test2);
		Chaos::Application::Get().GetRenderer().DrawQuad(new Chaos::Vec2(2.f, 2.f), new Chaos::Vec2(1.f, 1.f), big);
		Chaos::Application::Get().GetRenderer().DrawQuad(new Chaos::Vec2(x, y), new Chaos::Vec2(1.f, 1.f), player);
	}

	void OnEvent(Chaos::Event& event) override
	{
		if (event.GetEventType() == Chaos::EventType::KeyPressed)
		{

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