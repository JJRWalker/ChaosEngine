#pragma once

namespace Chaos
{
	class Vec3
	{
	public:
		Vec3(float x, float y, float z)
			: X(x), Y(y), Z(z) {};
		static Vec3 Zero() { return Vec3(0.0f, 0.0f, 0.0f); }

	public:
		float X, Y, Z;
	};
}