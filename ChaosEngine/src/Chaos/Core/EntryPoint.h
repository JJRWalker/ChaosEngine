#ifndef _ENTRY_POINT_DEFINED_H
#define _ENTRY_POINT_DEFINED_H

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

#endif // CHAOS_PLATFORM_WINDOWS

#endif // _ENTRY_POINT_DEFINED_H
