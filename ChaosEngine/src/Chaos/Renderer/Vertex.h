#pragma once
#include "Chaos/DataTypes/Vec3.h"
#include "Chaos/DataTypes/Vec2.h"

//generic data type for renderer vertex, could be used later on when implementing other rendering APIs or to draw UI elements along with PrimativeType

namespace Chaos
{
	class Vertex
	{
	public:
		Vertex(Vec2 pos, Vec3 color, Vec2 texCoord) : mPosition(pos), mColor(color), mTexCoord(texCoord) {};
		
		Vec2 GetPosition() { return mPosition; }
		Vec3 GetColor() { return mColor; }
		Vec2 GetTexCoord() { return mTexCoord; }

	private:
		Vec2 mPosition;
		Vec3 mColor;
		Vec2 mTexCoord;
	};
}