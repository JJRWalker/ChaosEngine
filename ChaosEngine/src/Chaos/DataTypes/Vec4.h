#pragma once

namespace Chaos
{
	class Vec4
	{
	public:
		Vec4(float x, float y, float z, float w)
			: X(x), Y(y), Z(z), W(w) {};

	public:
		float X, Y, Z, W;
	};
}