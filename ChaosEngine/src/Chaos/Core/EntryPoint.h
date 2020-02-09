#pragma once

#ifdef CHAOS_PLATFORM_WINDOWS

extern Chaos::Application* Chaos::CreateApplication();

int main(int argc, char** argv)
{

	//INITALISING LOG
	Chaos::Log::Init();

	LOGCORE_INFO("LOG INITALISED");

	auto app = Chaos::CreateApplication();
	app->Run();
	delete app;

}

#endif // 