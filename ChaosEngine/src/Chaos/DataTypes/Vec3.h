#pragma once

namespace Chaos
{
	class Vec3
	{
	public:
		Vec3(float x, float y, float z)
			: X(x), Y(y), Z(z) {};

	public:
		float X, Y, Z;
	};
}