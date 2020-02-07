
#include "Core/Chaos.h"


class GameApp : public Chaos::Application
{
public:
	GameApp()
	{
		
	}

	~GameApp()
	{

	}

};

Chaos::Application* Chaos::CreateApplication()
{
	return new GameApp();
}