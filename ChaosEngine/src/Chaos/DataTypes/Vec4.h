#pragma once

namespace Chaos
{
	class Vec4
	{
	public:
		Vec4(float x, float y, float z, float w)
			: X(x), Y(y), Z(z), W(w) {};
		static Vec4 Zero() { return Vec4(0.0f, 0.0f, 0.0f, 0.0f); }

	public:
		float X, Y, Z, W;
	};
}