
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
	Chaos::Ref<Chaos::Texture> player = Chaos::Texture::Create("../Game/textures/sprite-test.png", 1);
	Chaos::Ref<Chaos::Texture> floor = Chaos::Texture::Create("../Game/textures/Floor.jpg", 20);
	Chaos::Ref<Chaos::Texture> test = Chaos::Texture::Create("../Game/textures/test.png", 1);
	//Chaos::Ref<Chaos::Texture> big = Chaos::Texture::Create("../Game/textures/big-texture.png", 1);
	Chaos::Ref<Chaos::Texture> blank = Chaos::Texture::Create("", 1);
	Chaos::Ref<Chaos::Texture> test2 = Chaos::Texture::Create("../Game/textures/test2.png", 1);
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
		for (int x = 0; x < 100; ++x)
		{			
			for (int y = 0; y < 100; ++y)
			{
				//Chaos::Timer timer("GameApp: StressTest");
				renderer.DrawQuad(Chaos::Vec2(x - 20, y - 20), Chaos::Vec2(1.f, 1.f), floor);
			}
		}
		*/
		

		renderer.DrawQuad(Chaos::Vec2(0.f, 0.f), Chaos::Vec2(20.f, 20.f), floor);
		renderer.DrawQuad(Chaos::Vec2(4.f, 0.f), Chaos::Vec2(1.f, 1.f), blank);
		renderer.DrawQuad(Chaos::Vec2(3.f, 0.5f), Chaos::Vec2(1.f, 1.f), Chaos::Vec4(0.7, 0.7, 0.1, 0.4), blank);
		//renderer.DrawQuad(Chaos::Vec2(3.f, 0.f), Chaos::Vec2(1.f, 1.f), Chaos::Vec4(0.1,1,0.1,1), blank);
		//renderer.DrawQuad(Chaos::Vec2(2.f, 0.f), Chaos::Vec2(1.f, 1.f), test2);
		renderer.DrawQuad(Chaos::Vec2(x, y), Chaos::Vec2(1.f, 1.f), Chaos::Vec4(1.0, 0.1, 0.1, 0.9), player);
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