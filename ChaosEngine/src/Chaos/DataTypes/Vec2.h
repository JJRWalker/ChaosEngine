#pragma once

namespace Chaos
{
	class Vec2
	{
	public:
		//Constructors
		Vec2(float x, float y)
			: X(x), Y(y){};
		Vec2() { X = 0; Y = 0; }

		//Functions, nonstatic
		float Magnitude() { return sqrtf((X * X) + (Y * Y)); }
		Vec2 Normalised() { return *this / this->Magnitude(); }

#pragma region STATIC CLASS FUNCTIONS
		//static class functions
		static float Dot(Vec2 a, Vec2 b)
		{
			return a.X * b.X + a.Y * b.Y;
		}

		//distance between 2 points (gets vector representation and returns the magnitude of that vector
		static float Distance(Vec2 startPoint, Vec2 endPoint) {
			Vec2 vec = endPoint - startPoint; 
			return vec.Magnitude();
		}

		static float Angle(Vec2 from, Vec2 to) 
		{
			float theta = Dot(from, to) / (from.Magnitude() * to.Magnitude());
			theta = acos(theta);
			theta *= 57.2958f;	//convert to degrees

			//if cross x1y2 is less than x2y1 the rotation is counter clockwise
			if (to.X * from.Y < to.Y * from.X)
			{
				theta *= -1;
			}
			return theta;
		}

		static Vec2 Zero() { return Vec2(0.0f, 0.0f); }
#pragma endregion

#pragma region OPERATORS
		//addition operator, adds the two vector components to the other vector's componenets
		Vec2 operator + (const Vec2& other) const {
			float x = X + other.X;
			float y = Y + other.Y;

			return Vec2(x, y);
		};

		//subtraction operator, subtracts the other vector components from this vector's components
		Vec2 operator - (const Vec2& other) const {
			float x = X - other.X;
			float y = Y - other.Y;

			return Vec2(x, y);			
		};

		//division operator with float
		Vec2 operator / (const float divisor) const {
			float x = X / divisor;
			float y = Y / divisor;

			return Vec2(x, y);
		}

		Vec2 operator * (const float multiplier) const {
			float x = X * multiplier;
			float y = Y * multiplier;

			return Vec2(x, y);
		}
#pragma endregion

	public:
		union {
			float X;
			float Width;
		};
		union {
			float Y;
			float Height;
		};
	};
}