#pragma once

namespace Chaos
{
	class Vec3
	{
		public:
		Vec3(float x, float y, float z)
			: X(x), Y(y), Z(z) {};
		static Vec3 Zero() { return Vec3(0.0f, 0.0f, 0.0f); }
		
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