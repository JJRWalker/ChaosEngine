/* date = January 30th 2021 1:33 pm */

#ifndef _CAMERA_H
#define _CAMERA_H

#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/DataTypes/Vec3.h"
#include "Chaos/Nodes/Node.h"

#define GLM_FORCE_RADIANS
#include <GLM/glm/glm.hpp>	//Potentially create my own mat4 so glm is not required here

//Currently only an orthagraphic camera is supported, doesn't make much sense to use a perspective camera at this time in 2D development

namespace Chaos
{
	class Camera : public Node
	{
		public:
		Camera() { };
		
		// Inherited via Node
		virtual void Init() override;
		virtual void Update(float delta) override;
		virtual void Kill()  override;
		
		void SetBounds(Vec4 bounds) { m_bounds = bounds; Recalculate(); }
		Vec4 GetBounds() { return m_bounds; }
		
		float GetAspectRatio() { return m_aspectRatio; }
		void SetAspectRatio(float value) { m_aspectRatio = value; }
		
		Vec2 ScreenToWorld(Vec2 screenPoint);
		
		glm::mat4 GetProjection() { return m_projection; }
		glm::mat4 GetView() { return m_view; }
		glm::mat4 GetModel() { return m_model; }
		
		private:
		void Recalculate();
		
		private:
		Vec4 m_bounds = Vec4(-5.0f, 5.0f, -5.0f, 5.0f);
		float m_aspectRatio = 1.77f;
		
		glm::mat4 m_projection = glm::mat4();
		glm::mat4 m_view = glm::mat4(1.0f);
		glm::mat4 m_model = glm::mat4();
	};
}

#endif //_CAMERA_H
