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
		union {
			float X;
			float Left;
			float R;
		};
		union {
			float Y;
			float Right;
			float G;
		};
		union {
			float Z;
			float Bottom;
			float B;
		};
		union {
			float W;
			float Top;
			float A;
		};
	};
}