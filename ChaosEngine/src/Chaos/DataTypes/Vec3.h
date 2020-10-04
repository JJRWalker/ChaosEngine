#pragma once

namespace Chaos
{
	class Vec3
	{
		public:
		Vec3(float x, float y, float z)
			: X(x), Y(y), Z(z) {};
		static Vec3 Zero() { return Vec3(0.0f, 0.0f, 0.0f); }
		
		Vec3 operator - (const Vec3 other) const
		{
			float x = X - other.X;
			float y = Y - other.Y;
			float z = Z - other.Z;
			
			return Vec3(x, y, z);
		}
		
		Vec3 operator + (const Vec3 other) const
		{
			float x = X + other.X;
			float y = Y + other.Y;
			float z = Z + other.Z;
			
			return Vec3(x, y, z);
		}
		
		Vec3 operator += (const Vec3& other){
			X += other.X;
			Y += other.Y;
			return *this;
		}
		
		public:
		float X, Y, Z;
	};
}