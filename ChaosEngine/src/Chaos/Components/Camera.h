#pragma once

#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/DataTypes/Vec3.h"

namespace Chaos
{
	class Camera
	{
	public:
		Camera() {};
		Camera(Vec3 positon, Vec3 rotation, Vec4 bounds): mPosition(positon), mRotation(rotation), mBounds(bounds) {};

		void SetPosition(Vec3 position) { mPosition = position; }
		void SetRotation(Vec3 rotation) { mRotation = rotation; }
		void SetBounds(Vec4 bounds) { mBounds = bounds; }

		Vec3 GetPosition() { return mPosition; }
		Vec3 GetRoation() { return mRotation; }
		Vec4 GetBounds() { return mBounds; }

	private:
		Vec3 mPosition = Vec3::Zero();
		Vec3 mRotation = Vec3::Zero();
		Vec4 mBounds = Vec4(-5.0f, 5.0f, -5.0f, 5.0f);
	};
}