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
		Camera(Vec3 positon, Vec3 rotation, Vec4 bounds) : m_position(positon), m_rotation(rotation), m_bounds(bounds) { Recalculate(); };

		void SetPosition(Vec3 position) { m_position = position; Recalculate(); }
		void SetRotation(Vec3 rotation) { m_rotation = rotation; Recalculate(); }
		void SetBounds(Vec4 bounds) { m_bounds = bounds; Recalculate(); }

		Vec3 GetPosition() { return m_position; }
		Vec3 GetRoation() { return m_rotation; }
		Vec4 GetBounds() { return m_bounds; }

		glm::mat4 GetProjection() { return m_projection; }
		glm::mat4 GetView() { return m_view; }

	private:
		void Recalculate();

	private:
		Vec3 m_position = Vec3::Zero();
		Vec3 m_rotation = Vec3::Zero();
		Vec4 m_bounds = Vec4(-5.0f, 5.0f, -5.0f, 5.0f);

		glm::mat4 m_projection = glm::mat4();
		glm::mat4 m_view = glm::mat4();
	};
}