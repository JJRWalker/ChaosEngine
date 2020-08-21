
#include <Chaos.h>
#include <Chaos/Events/KeyEvent.h>
#include <Chaos/Core/EntryPoint.h>


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
	Chaos::Ref<Chaos::Texture> blank = Chaos::Texture::GetBlank();
	Chaos::Ref<Chaos::Texture> test2 = Chaos::Texture::Create("../Game/textures/test2.png");
	//Test entity
	Chaos::Entity entity;
	Chaos::Entity entity2;
	bool switched = false;

	Chaos::Render* render;

	float x = 0;
	float y = 0;
	float z = 0;

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
		if (Chaos::Input::IsKeyPressed(KEY_Q))
		{
			z -= (moveSpeed * deltaTime);
		}
		else if (Chaos::Input::IsKeyPressed(KEY_E))
		{
			z += (moveSpeed * deltaTime);
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
		renderer.DrawQuad(Chaos::Vec3::Zero(), Chaos::Vec2(20.f, 20.f), floor, 30); //this one tiles
		renderer.DrawQuad(Chaos::Vec3::Zero(), Chaos::Vec2(1.f, 1.f), floor);	//this one does not

		//Coloured sprite example
		renderer.DrawQuad(Chaos::Vec3::Zero(), Chaos::Vec2(1.f, 1.f), Chaos::Vec4(1.0f, 0.1f, 0.1f, 0.9f), player);	//tinted red 

		//sub sprite example
		renderer.DrawQuad(Chaos::Vec3(4.f, 0.f, 0.1f), Chaos::Vec2(1.f, 1.f), playersub);	//draws the bottom right quarter of the player sprite


		//Entity test
		entity.GetTransform()->Position() = Chaos::Vec3(x,y,z); //changes the entity's position to be the same as the cameras. Changing this changes where the Render component renders the entity 
		//LOGCORE_TRACE("{0} {1} {2}", entity.GetTransform()->Position().X, entity.GetTransform()->Position().Y, entity.GetTransform()->Position().Z);
		entity2.GetTransform()->Position().Y += 0.1f * deltaTime;
		entity.Update();
		entity2.Update();
	}

	void OnAttach() override
	{
		//Stuff to do on start goes here
		//Adding component to entity example
		entity.AddComponent<Chaos::Render>();
		entity.GetComponent<Chaos::Render>()->SetTexture(test);	//setting the texture for the render component (defaults to blank) 
		entity2.AddComponent<Chaos::Render>();
		entity2.GetComponent<Chaos::Render>()->SetTexture(player);	//setting the texture for the render component (defaults to blank) 
		render = entity.GetComponent <Chaos::Render> ();
		Chaos::Render& refrender = Chaos::Render();
		if (entity2.TryGetComponent(refrender))
		{
			LOGINFO("Got render component from entity");
			refrender.SetTexture(test);
		}
		render->SetTexture(player);
	}

	void OnDetach() override
	{
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