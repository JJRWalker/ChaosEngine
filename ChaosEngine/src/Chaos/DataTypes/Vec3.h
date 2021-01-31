#pragma once

namespace Chaos
{
	class Vec3
	{
		public:
		Vec3() : X(0), Y(0), Z(0) {}
		Vec3(float x, float y, float z)
			: X(x), Y(y), Z(z) {};
		static Vec3 Zero() { return Vec3(0.0f, 0.0f, 0.0f); }
		
		float Magnitude() {return sqrtf((X * X) + (Y * Y) + (Z * Z));}
		
		inline Vec3 operator - (const Vec3 other) const
		{
			float x = X - other.X;
			float y = Y - other.Y;
			float z = Z - other.Z;
			
			return Vec3(x, y, z);
		}
		
		inline Vec3 operator + (const Vec3 other) const
		{
			float x = X + other.X;
			float y = Y + other.Y;
			float z = Z + other.Z;
			
			return Vec3(x, y, z);
		}
		
		inline Vec3 operator = (const Vec3& other){
			X = other.X;
			Y = other.Y;
			Z = other.Z;
			return *this;
		}
		
		inline Vec3 operator += (const Vec3& other){
			X = X + other.X;
			Y = Y + other.Y;
			Z = Z + other.Z;
			return *this;
		}
		
		public:
		float X, Y, Z;
	};
}