#pragma once

#include "Core.h"

namespace Chaos
{
	class Application
	{
	public:

		Application();
		virtual ~Application();

		void Run();

	};

	//defined in client
	Application* CreateApplication();
}