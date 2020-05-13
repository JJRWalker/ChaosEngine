#pragma once

namespace Chaos
{
	class Vec2
	{
	public:
		Vec2(float x, float y)
			: X(x), Y(y){};
		static Vec2 Zero() { return Vec2(0.0f, 0.0f); }

	public:
		float X, Y;
	};
}