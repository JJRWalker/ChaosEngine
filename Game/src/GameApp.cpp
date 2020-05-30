
#include <Chaos.h>
#include <Chaos/Events/KeyEvent.h>
#include <Chaos/Components/Camera.h>


//NOTE: this is a quick and dirty implementation to test functionality, not representitive of the product

class ExampleLayer : public Chaos::Layer
{
public:
	ExampleLayer()
		:Layer("Example") {}

	float moveSpeed = 1.f;
	int xDir = 0;
	int yDir = 0;

	//Defining textures
	Chaos::Ref<Chaos::Texture> player = Chaos::Texture::Create("../Game/textures/sprite-test.png");
	Chaos::Ref<Chaos::SubTexture> playersub = Chaos::SubTexture::Create(player, Chaos::Vec2(1, 1), Chaos::Vec2(160, 160));
	Chaos::Ref<Chaos::Texture> floor = Chaos::Texture::Create("../Game/textures/Floor.jpg");
	Chaos::Ref<Chaos::Texture> test = Chaos::Texture::Create("../Game/textures/test.png");
	Chaos::Ref<Chaos::Texture> blank = Chaos::Texture::Create("");
	Chaos::Ref<Chaos::Texture> test2 = Chaos::Texture::Create("../Game/textures/test2.png");
	float x = 0;
	float y = 0;

	//reference to renderer to be abstracted
	Chaos::Renderer& renderer = Chaos::Application::Get().GetRenderer();

	void OnUpdate(float deltaTime) override
	{
		//Inefficent input checking, TODO: change to axis based input system / expand
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

		//Delta time is passed to each layer by the Application class 
		x += xDir * (moveSpeed * deltaTime);
		y += yDir * (moveSpeed * deltaTime);

		//Stress test
		//Inefficient, these draw calls should be made on the start of the scene and stored in a static buffer (one that isn't recreated every frame) as they do not move
		/*
		for (int x = 0; x < 100; ++x)
		{			
			for (int y = 0; y < 100; ++y)
			{
				//Chaos::Timer timer("GameApp: StressTest"); //Timer example, logs to the console when it leaves scope
				renderer.DrawQuad(Chaos::Vec2(x - 20, y - 20), Chaos::Vec2(1.f, 1.f), floor);
			}
		}
		*/
		
		//Tiling example
		renderer.DrawQuad(Chaos::Vec2(0.f, 0.f), Chaos::Vec2(20.f, 20.f), floor, 30); //this one tiles
		renderer.DrawQuad(Chaos::Vec2(0.f, 0.f), Chaos::Vec2(1.f, 1.f), floor);	//this one does not

		//Coloured sprite example
		renderer.DrawQuad(Chaos::Vec2(0, 0), Chaos::Vec2(1.f, 1.f), Chaos::Vec4(1.0f, 0.1f, 0.1f, 0.9f), player);	//tinted red 

		//sub sprite example
		renderer.DrawQuad(Chaos::Vec2(4.f, 0.f), Chaos::Vec2(1.f, 1.f), playersub);	//draws the bottom right quarter of the player sprite

		//Camera movement example (not fully implemented)
		Chaos::Application::Get().GetMainCamera().SetPosition(Chaos::Vec3( x, y, 0.f));	// moves camera based on directional input from W A S D keys 
	}

	void OnAttach() override
	{
		//Stuff to do on start goes here
	}

	void OnEvent(Chaos::Event& event) override
	{
		//On every window event
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