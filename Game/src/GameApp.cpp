
#include <Chaos.h>
#include <Chaos/Events/KeyEvent.h>
#include <Chaos/Components/Camera.h>

class ExampleLayer : public Chaos::Layer
{
public:
	ExampleLayer()
		:Layer("Example") {}

	float moveSpeed = 1.f;
	int xDir = 0;
	int yDir = 0;
	Chaos::Ref<Chaos::Texture> player = Chaos::Texture::Create("../Game/textures/sprite-test.png");
	Chaos::Ref<Chaos::Texture> floor = Chaos::Texture::Create("../Game/textures/Floor.jpg");
	Chaos::Ref<Chaos::Texture> test = Chaos::Texture::Create("../Game/textures/test.png");
	//Chaos::Ref<Chaos::Texture> big = Chaos::Texture::Create("../Game/textures/big-texture.png", 1);
	Chaos::Ref<Chaos::Texture> blank = Chaos::Texture::Create("");
	Chaos::Ref<Chaos::Texture> test2 = Chaos::Texture::Create("../Game/textures/test2.png");
	float x = 0;
	float y = 0;

	Chaos::Camera* cam1 = new Chaos::Camera();
	Chaos::Camera* cam2 = new Chaos::Camera();

	Chaos::Renderer& renderer = Chaos::Application::Get().GetRenderer();

	bool pressed = false;
	bool switched = false;

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

		if (Chaos::Input::IsKeyPressed(KEY_E) && pressed == false)
		{
			if (switched)
			{
				Chaos::Application::Get().SetMainCamera(*cam1);
				switched = false;
			}
			else
			{
				Chaos::Application::Get().SetMainCamera(*cam2);
				switched = true;
			}
			pressed = true;
		}
		else if (Chaos::Input::IsKeyReleased(KEY_E) && pressed == true)
		{
			pressed = false;
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
		

		renderer.DrawQuad(Chaos::Vec2(0.f, 0.f), Chaos::Vec2(20.f, 20.f), floor, 30);
		renderer.DrawQuad(Chaos::Vec2(0.f, 0.f), Chaos::Vec2(1.f, 1.f), floor);
		renderer.DrawQuad(Chaos::Vec2(4.f, 0.f), Chaos::Vec2(1.f, 1.f), blank);
		renderer.DrawQuad(Chaos::Vec2(3.f, 0.5f), Chaos::Vec2(1.f, 1.f), Chaos::Vec4(0.7f, 0.7f, 0.1f, 0.4f), blank);
		//renderer.DrawQuad(Chaos::Vec2(3.f, 0.f), Chaos::Vec2(1.f, 1.f), Chaos::Vec4(0.1,1,0.1,1), blank);
		//renderer.DrawQuad(Chaos::Vec2(2.f, 0.f), Chaos::Vec2(1.f, 1.f), test2);
		renderer.DrawQuad(Chaos::Vec2(0, 0), Chaos::Vec2(1.f, 1.f), Chaos::Vec4(1.0f, 0.1f, 0.1f, 0.9f), player);

		Chaos::Application::Get().GetCamera().SetPosition(Chaos::Vec3( x, y, 0.f));
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