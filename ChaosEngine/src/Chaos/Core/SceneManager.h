#pragma once
#include "Scene.h"

//Singleton class that stores the current scene and handles loading / saving of the scene along with other management functions

namespace Chaos
{
	class SceneManager
	{
	public:
		static void Load(Scene& scene) { s_currentScene = &scene; s_currentScene->StartScene(); }

		static Scene* GetScene() { return s_currentScene; }

	private:
		static Scene* s_currentScene;
	};
}