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
		void Destroy();
		void OnStart();
		void OnEnd();
		void OnUpdate(float delta);
		void OnFixedUpdate(float delta);
		
		void Save(const char* filepath);
		void Load(const char* filepath);
		
		
		static Level* Get();
		
		public:
		Node* Nodes[MAX_NODES][MAX_CHILD_NODES];
		size_t NodeCount = 0;
		Camera* MainCamera = nullptr;
		
		bool PendingDestroy = false;
		
		static Level* s_instance;
		
		private:
		Camera* p_mainCamera = nullptr;
	};
}

#endif //_LEVEL_H
