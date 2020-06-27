#pragma once

#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/DataTypes/Vec3.h"

#define GLM_FORCE_RADIANS
#include <GLM/glm/glm.hpp>	//Potentially create my own mat4 so glm is not required here

//Currently only an orthagraphic camera is supported, doesn't make much sense to use a perspective camera at this time in 2D development
namespace Chaos
{
	//Eventually this class will inherit from a Component interface so it can be attached to a game object a la unity
	class Camera
	{
	public:
		Camera() {};
		Camera(Vec3 positon, Vec3 rotation, Vec4 bounds) : mPosition(positon), mRotation(rotation), mBounds(bounds) { Recalculate(); };

		void SetPosition(Vec3 position) { mPosition = position; Recalculate(); }
		void SetRotation(Vec3 rotation) { mRotation = rotation; Recalculate(); }
		void SetBounds(Vec4 bounds) { mBounds = bounds; Recalculate(); }

		Vec3 GetPosition() { return mPosition; }
		Vec3 GetRoation() { return mRotation; }
		Vec4 GetBounds() { return mBounds; }

		glm::mat4 GetProjection() { return mProjection; }
		glm::mat4 GetView() { return mView; }

	private:
		void Recalculate();

	private:
		Vec3 mPosition = Vec3::Zero();
		Vec3 mRotation = Vec3::Zero();
		Vec4 mBounds = Vec4(-5.0f, 5.0f, -5.0f, 5.0f);

		glm::mat4 mProjection;
		glm::mat4 mView;
	};
}