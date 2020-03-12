#pragma once
#include "Vec3.h"
#include "Vec2.h"

namespace Chaos
{
	class Vertex
	{
	public:
		Vertex() {};
		virtual ~Vertex() {};
		
		virtual Vec2 GetPosition() = 0;
		virtual Vec3 GetColor() = 0;
		virtual Vec2 GetTexCoord() = 0;

		virtual void SetPosition(Vec2 position) = 0;
		virtual void SetColor(Vec3 color) = 0;
		virtual void SetTexCoord(Vec2 coord) = 0;

	};
}