/* date = January 30th 2021 11:26 am */

#ifndef _LEVEL_H
#define _LEVEL_H

#include "Chaos/Core/Core.h"

namespace Chaos
{
	class Node;
	class Camera;
	class Level
	{
		public:
		Level();
		~Level();
		void OnStart();
		void OnEnd();
		void OnUpdate(float delta);
		void OnFixedUpdate(float delta);
		void OnEditorUpdate(float delta);
		
		static void Save(const char* filepath);
		static void Load(const char* filepath);
		
		static Level* Get();
		
		private:
		static void InternalLoad();
		
		public:
		Node* Nodes[MAX_NODES];
		size_t NodeCount = 0;
		Camera* MainCamera = nullptr;
		
		static Level* s_instance;
		
		private:
		std::string m_filepathToLoad;
		
		Camera* p_mainCamera = nullptr;
	};
}

#endif //_LEVEL_H
