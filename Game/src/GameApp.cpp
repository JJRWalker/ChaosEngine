
#include <Chaos.h>
#include <Chaos/Events/KeyEvent.h>
#include <Chaos/Core/EntryPoint.h>
#include "Components/PlayerController.h"
#include "Chaos/Entity/Components/CellularAutomata.h"
#include <Chaos/Entity/Components/Animator.h>
#include <Chaos/Entity/Components/Camera.h>

//NOTE: this is a quick and dirty implementation to test functionality, not representitive of the product

class ExampleLayer : public Chaos::Layer
{
public:
	ExampleLayer()
		:Layer("Example") {}

	//Test entity
	Chaos::Entity player;

	//Ray
	Chaos::RayHit2DInfo* rayHitInfo;

	Chaos::Vec2 line = Chaos::Vec2(0.0f, 1.0f);

	static const int MAP_SIZE_X = 30;
	static const int MAP_SIZE_Y = 30;
	//char m_map[MAP_SIZE_X][MAP_SIZE_Y] = {
	//										'w', 'w', 'w', 'w' ,'w', 'w', 'w', 'w', 'w', 'w' ,'w', 'w', 'w', 'w', 'w', 'w' ,'w', 'w', 'w', 'w', 
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', 'w', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', 'w', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', 'w', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', 'w', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0', '0' ,'0', '0', '0', 'w',
	//										'w', 'w', 'w', 'w' ,'w', 'w', 'w', 'w', 'w', 'w' ,'w', 'w', 'w', 'w', 'w', 'w' ,'w', 'w', 'w', 'w'
	//
	//									};
	char m_map[MAP_SIZE_X][MAP_SIZE_Y];

	//Raycaster
	std::vector<Chaos::Entity*> raycastDisplay;
	const int RESOLUTION = 60;
	const float FOV = 30;
	const float SIGHT_DISTANCE = 20.0f;
	const float COLOUR_DROPOFF = 10.0f;

	void OnUpdate(float deltaTime) override
	{
		//Chaos::Application::Get().GetRenderer().DrawQuad(Chaos::Vec3(0, 0, 1), Chaos::Vec2(1, 1), Chaos::SubTexture::Create("../Game/textures/floor.jpg", Chaos::Vec2(0, 0), Chaos::Vec2(16, 16)));
	}

	void OnAttach() override
	{
		player.AddComponent<Chaos::Animator>();
		Chaos::Animation anim;
		anim.SpriteSheet = Chaos::Texture::Create("../Game/textures/test-anim.png");
		anim.Loop = true;
		anim.TotalFrames = 9;
		anim.PlaybackSpeed = 1;
		anim.FrameRate = 9;
		anim.FrameSize.X = 32.f;
		anim.FrameSize.Y = 32.f;
		anim.StartFrame = 1;

		player.GetComponent<Chaos::Animator>()->SetAnimation(anim);
		player.GetComponent<Chaos::Animator>()->Play();



		player.GetTransform()->Position() = Chaos::Vec3(0, 0, 1);
		player.GetTransform()->Scale() = Chaos::Vec2(10, 10);
	}

	void OnDetach() override
	{
	}

	void OnEvent(Chaos::Event& event) override
	{
		//On every window event
	}

	void GenerateLevel()
	{
		for (int xpos = 0; xpos < MAP_SIZE_X; ++xpos)
		{
			for (int ypos = 0; ypos < MAP_SIZE_Y; ++ypos)
			{
				if ((xpos == 0 || xpos == MAP_SIZE_X - 1 || ypos == 0 || ypos == MAP_SIZE_Y - 1) || ((xpos < 8 && ypos < 4) || (xpos > 22 && ypos > 22)))
				{
					m_map[xpos][ypos] = 'w';
				}
			}
		}

		for (int xpos = 0; xpos < MAP_SIZE_X; ++xpos)
		{
			for (int ypos = 0; ypos < MAP_SIZE_Y; ++ypos)
			{
				if (m_map[xpos][ypos] == 'w')
				{
					Chaos::Entity* wall = new Chaos::Entity("Wall");
					wall->AddComponent<Chaos::BoxCollider2D>();
					wall->GetTransform()->Position() = Chaos::Vec3(xpos, ypos, 0.f);
				}
			}
		}

		//setting up level walls
		for (int xpos = 0; xpos < MAP_SIZE_X; ++xpos)
		{
			for (int ypos = 0; ypos < MAP_SIZE_Y; ++ypos)
			{
				if (m_map[xpos][ypos] == 'w')
				{
					Chaos::Entity* wall = new Chaos::Entity("Wall");
					wall->AddComponent<Chaos::BoxCollider2D>();
					wall->GetTransform()->Position() = Chaos::Vec3(xpos, ypos, 0.f);
				}
			}
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