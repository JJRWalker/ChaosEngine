/* date = January 30th 2021 11:26 am */

#ifndef _LEVEL_H
#define _LEVEL_H

#include "Chaos/Core/Core.h"
#include "Chaos/DataTypes/Vec4.h"

namespace Chaos
{
	// Data sent to the GPU via the renderer
	// Should be in uniform strides
	// Up to the shader how it interperates this data
	struct GPULevelData
	{
		Vec4 FogColour = { 0.0f, 0.0f, 0.0f, 0.0f };
		Vec4 FogDistances = { 0.0f,  0.0f, 0.0f, 0.0f};
		Vec4 AmbiantColour = { 1.0f, 1.0f, 1.0f, 1.0f };
		Vec4 SunlightDirection = { 0.0f, 0.0f, 0.0f, 0.0f };
		Vec4 SunlightColour = { 0.0, 0.0f, 0.0f, 0.0f };
		Vec4 LightingData = { 0.0f, 0.0f, 0.0f, 0.0f }; 
	};
	
	class Node;
	class Camera;
	class Level
	{
		const uint32_t LEVEL_SAVE_VERSION_0 = 0;
		
		public:
		Level();
		~Level();
		void OnStart();
		void OnEnd();
		void OnUpdate(float delta);
		void OnFixedUpdate(float delta);
		void OnEditorUpdate(float delta);
		
		static void Clear();
		static void Save(const char* filepath);
		static void Load(const char* filepath);
		
		static Level* Get();
		
		private:
		static void InternalLoad();
		
		public:
		Node* Nodes[MAX_NODES];
		size_t NodeCount = 0;
		Camera* MainCamera = nullptr;
		GPULevelData GraphicalData;
		
		static Level* s_instance;
		
		private:
		std::string m_filepathToLoad;
		
		uint32_t m_levelVersion = LEVEL_SAVE_VERSION_0;
	};
}

#endif //_LEVEL_H
