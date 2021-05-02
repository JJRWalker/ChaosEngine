/* date = April 26th 2021 0:29 pm */

#ifndef _MESH_H
#define _MESH_H

#include <string>

#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec3.h"

namespace Chaos
{
	struct Vertex
	{
		Vec3 Position;
		Vec3 Normal;
		Vec3 Colour;
		Vec2 UV;
	};
	
	class Mesh
	{
		public:
		bool LoadFromOBJ(const char* filename);
		
		public:
		std::string Name = "";
		std::vector<Vertex> Vertices;
	};
}

#endif //_MESH_H
