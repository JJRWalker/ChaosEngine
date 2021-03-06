#pragma once
#include "chaospch.h"
#include "Chaos/Core/Math.h"

#define MIN_FLOAT_COMPONENT_VALUE 0.000000001f

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
		float Magnitude() { return sqrtf((X * X) + (Y * Y));}
		Vec2 Normalised() { 
			float x = 0;
			float y = 0;
			if (X != 0)
				x = X / sqrtf((X * X) + (Y * Y));
			if (Y != 0)
				y = Y / sqrtf((X * X) + (Y * Y));
			return Vec2(x,y);
		}
		
		
		Vec2 ClampMagnitude(float max)
		{
			return this->Normalised() * std::min(this->Magnitude(), max);
		}
		
		
		Vec2 Abs()
		{
			return Vec2(abs(X), abs(Y));
		}
		
		
		void Rotate(float degrees)
		{
			Vec2 result = Vec2::Zero();
			degrees *= -0.0174533f;
			result.X = (X * cos(degrees)) - (Y * sin(degrees));
			result.Y = (X * sin(degrees)) + (Y * cos(degrees));
			
			*this = result;
		}
		
		
		// treats this vec2 as a point against another that it treats the other two as a point and dimensions
		bool InsideBounds(Vec2 origin, Vec2 bounds)
		{
			float halfWidth = bounds.X * 0.5f;
			float halfHeight = bounds.Y * 0.5f;
			
			return X >= origin.X - halfWidth &&
				X<= origin.X + halfWidth &&
				Y>= origin.Y - halfHeight &&
				Y<= origin.Y + halfHeight;
		}
		
		
		bool IntersectsBounds(Vec2 bounds, Vec2 otherOrigin, Vec2 otherBounds)
		{
			bool left = otherOrigin.X - otherBounds.X * 0.5f > X + bounds.X * 0.5f;
			bool right = otherOrigin.X + otherBounds.X * 0.5f < X - bounds.X * 0.5f;
			bool bottom = otherOrigin.Y - otherBounds.Y * 0.5f > Y + bounds.Y * 0.5f;
			bool top = otherOrigin.Y + otherBounds.Y * 0.5f < Y - bounds.Y * 0.5f;
			return !(left || right || bottom || top); 
		}
		
		
#pragma region STATIC CLASS FUNCTIONS
		//static class functions
		static float Dot(Vec2& a, Vec2& b)
		{
			return a.X * b.X + a.Y * b.Y;
		}
		
		//distance between 2 points (gets vector representation and returns the magnitude of that vector
		static float Distance(Vec2 startPoint, Vec2 endPoint) {
			Vec2 vec = endPoint - startPoint; 
			return vec.Magnitude();
		}
		
		static float Angle(Vec2& from, Vec2& to) 
		{
			float theta = Dot(from, to) / (from.Magnitude() * to.Magnitude());
			
			theta = acos(theta); // no need to convert to degrees, we take all angles in rads
			
			//sometimes the angle may be too small to be contained in a float, in this case, or any other invalid operation, return 0
			if (isnan(theta))
			{
				return 0.0f;
			}
			
			//if cross x1y2 is less than x2y1 the rotation is counter clockwise
			if (to.X * from.Y < to.Y * from.X)
			{
				theta *= -1;
			}
			return theta;
		}		
		
		//lerps the vector between the start and end given a delta (value from 0 to 1)
		static Vec2 Lerp (Vec2& start, Vec2& end, float delta)
		{
			if (delta >= 1)
				return end;
			Vec2 distanceVector = end - start;
			distanceVector *= delta;
			
			return start + distanceVector;
		}
		
		static Vec2 Zero() { return Vec2(0.0f, 0.0f); }
#pragma endregion
		
#pragma region OPERATORS
		//addition operator, adds the two vector components to the other vector's componenets
		inline Vec2 operator + (const Vec2& other) const {
			float x = X + other.X;
			float y = Y + other.Y;
			
			return Vec2(x, y);
		};
		
		//subtraction operator, subtracts the other vector components from this vector's components
		inline  Vec2 operator - (const Vec2& other) const {
			float x = X - other.X;
			float y = Y - other.Y;
			
			return Vec2(x, y);			
		};
		
		inline Vec2 operator -= (const Vec2& other){
			X -= other.X;
			Y -= other.Y;
			return *this;
		}
		
		//division operator with float
		inline Vec2 operator / (const float& divisor) {
			float x = 0;
			float y = 0;
			if (abs(X) > MIN_FLOAT_COMPONENT_VALUE)
				x = X / divisor;
			if (abs(Y) > MIN_FLOAT_COMPONENT_VALUE)
				y = Y / divisor;
			
			return Vec2(x, y);
		}
		
		inline Vec2 operator /= (const float& divisor) {
			float x = 0;
			float y = 0;
			
			if (abs(X) > MIN_FLOAT_COMPONENT_VALUE)
				x= X / divisor;
			if (abs(Y) > MIN_FLOAT_COMPONENT_VALUE)
				y = Y / divisor;
			X = x;
			Y = y;
			return *this;
		}
		
		inline Vec2 operator * (const float& multiplier) {
			float x = X * multiplier;
			float y = Y * multiplier;
			
			return Vec2(x, y);
		}
		
		inline Vec2 operator *= (const float& multiplier) {
			if (abs(X) > MIN_FLOAT_COMPONENT_VALUE)
				X = X * multiplier;
			else
				X = 0;
			if (abs(Y) > MIN_FLOAT_COMPONENT_VALUE)
				Y = Y * multiplier;
			else
				Y = 0;
			
			return *this;
		}
		
		inline Vec2 operator * (const Vec2& other) const {
			float x = X * other.X;
			float y = Y * other.Y;
			
			return Vec2(x, y);
		}
		
		inline Vec2 operator = (const Vec2& other) {
			X = other.X;
			Y = other.Y;
			return *this;
		}
		
		inline Vec2 operator += (const Vec2& other) {
			X = X + other.X;
			Y = Y + other.Y;
			return *this;
		}
		
		inline bool operator == (const Vec2& other){
			return X == other.X && Y == other.Y;
		}
		
		inline bool operator != (const Vec2& other){
			return X != other.X || Y != other.Y;
		}
		
		std::string ToString() const
		{
			std::stringstream ss;
			ss << X << "," << Y;
			return ss.str();
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