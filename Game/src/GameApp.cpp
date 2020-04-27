
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

	Chaos::Renderer& renderer = Chaos::Application::Get().GetRenderer();

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

		//Stress test 
		/*
		for (int x = 0; x < 20; ++x)
		{			
			for (int y = 0; y < 20; ++y)
			{
				Chaos::Timer timer("GameApp: StressTest");
				renderer.DrawQuad(new Chaos::Vec2(x - 20, y - 20), new Chaos::Vec2(1.f, 1.f), floor);
			}
		}
		*/

		renderer.DrawQuad(new Chaos::Vec2(0.f, 0.f), new Chaos::Vec2(20.f, 20.f), floor);
		renderer.DrawQuad(new Chaos::Vec2(4.f, 0.f), new Chaos::Vec2(1.f, 1.f), test);
		renderer.DrawQuad(new Chaos::Vec2(3.f, 0.f), new Chaos::Vec2(1.f, 1.f), test);
		renderer.DrawQuad(new Chaos::Vec2(2.f, 0.f), new Chaos::Vec2(1.f, 1.f), test2);
		renderer.DrawQuad(new Chaos::Vec2(x, y), new Chaos::Vec2(1.f, 1.f), player);
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