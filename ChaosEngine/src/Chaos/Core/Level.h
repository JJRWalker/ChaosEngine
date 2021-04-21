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
		void Start();
		void OnUpdate(float delta);
		void OnFixedUpdate(float delta);
		
		void Save(const char* filepath);
		void Load(const char* filepath);
		
		Camera* MainCamera();
		
		static Level* Get();
		
		public:
		Node* Nodes[MAX_NODES][MAX_CHILD_NODES];
		size_t NodeCount = 0;
		
		private:
		Camera* p_mainCamera = nullptr;
		static Level* s_instance;
	};
}

#endif //_LEVEL_H
