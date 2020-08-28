
#include <Chaos.h>
#include <Chaos/Events/KeyEvent.h>
#include <Chaos/Core/EntryPoint.h>
#include "Components/PlayerController.h"
#include "Chaos/Entity/Components/CellularAutomata.h"


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
	const int RESOLUTION = 100;
	const float FOV = 30;
	const float SIGHT_DISTANCE = 10.0f;
	const float COLOUR_DROPOFF = 10.0f;

	void OnUpdate(float deltaTime) override
	{
		//Vision / rendering
		line = player.GetComponent<PlayerController>()->GetForward();
		line.Rotate(-FOV / 2);

		for (int i = 0; i < RESOLUTION; ++i)
		{
			line.Rotate(FOV / RESOLUTION);

			rayHitInfo = Chaos::Ray2D::Cast(Chaos::Vec2(player.GetTransform()->Position().X, player.GetTransform()->Position().Y), line, SIGHT_DISTANCE);

			if (rayHitInfo)
			{
				//Chaos::Ray2D::DrawRay(Chaos::Vec2(x, y), line, rayHitInfo->Distance);
				float distance = rayHitInfo->Distance;

				float a = Chaos::Vec2::Angle(player.GetComponent<PlayerController>()->GetForward(), line);

				if (a != 0)
				{
					float r = cos(a * Chaos::Math::DEGREES_TO_RADS);
					distance *= r;
				}
				float distanceScalar = (SIGHT_DISTANCE - distance) / SIGHT_DISTANCE;

				raycastDisplay[i]->GetTransform()->Scale().Y = distanceScalar;

				float colour =  (SIGHT_DISTANCE / (distance * distance));
				//LOGTRACE("{0}", colour);
				colour /= COLOUR_DROPOFF;
				raycastDisplay[i]->GetComponent<Chaos::UIImage>()->Colour() = Chaos::Vec4(colour, colour, colour, 1.0f);
				raycastDisplay[i]->GetTransform()->Position().Z = SIGHT_DISTANCE - distance;
			}
			else
			{
				//Chaos::Ray2D::DrawRay(Chaos::Vec2(x, y), line, 10);
				raycastDisplay[i]->GetTransform()->Scale().Y = 0;
			}
		}
	}

	void OnAttach() override
	{
		//Stuff to do on start goes here
		//Adding component to entity example
		player.AddComponent<PlayerController>();
		player.GetTransform()->Position() = Chaos::Vec3(5.f, 5.f, 0);


		Chaos::Entity* backGround = new Chaos::Entity();
		backGround->AddComponent<Chaos::UIImage>();
		backGround->GetTransform()->Position() = Chaos::Vec3(0.5f, 0.5f, 0.0f);
		backGround->GetComponent<Chaos::UIImage>()->Colour() = Chaos::Vec4(0.001f, 0.001f, 0.001f, 1.0f);
		backGround->GetTransform()->Scale().X = Chaos::Application::Get().GetMainCamera()->GetAspectRatio();


		GenerateLevel();


		//Setting up raycaster display 
		for (int i = 0; i < RESOLUTION; ++i)
		{
			Chaos::Entity* ent = new Chaos::Entity("RayCastDisplay");
			ent->AddComponent<Chaos::UIImage>();
			ent->GetTransform()->Position() = Chaos::Vec3(i * (1.0f / RESOLUTION), 0.5f, 1000);
			ent->GetTransform()->Scale() = Chaos::Vec2((1.0f / RESOLUTION) * Chaos::Application::Get().GetMainCamera()->GetAspectRatio(), 1.0f);
			ent->GetComponent<Chaos::UIImage>()->Colour() = Chaos::Vec4(0.1f, 0.1f, 0.1f, 1.0f);
			raycastDisplay.push_back(ent);
		}
		//m_scene.AddEntity(&uiImage);

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
				if (xpos == 0 || xpos == MAP_SIZE_X - 1 || ypos == 0 || ypos == MAP_SIZE_Y - 1)
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