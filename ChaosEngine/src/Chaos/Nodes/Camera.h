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
	enum EProjectionType
	{
		PERSPECTIVE = 0,
		ORTHAGRAPHIC = 1
	};
	
	class Camera : public Node
	{
		public:
		Camera();
		~Camera();
		
		// Inherited via Node
		virtual void OnStart() override;
		virtual void OnUpdate(float delta) override;
		
		Binary SaveToBinary() override;
		size_t LoadFromBinary(char* data) override;
		
		void OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle);
		
		void SetBounds(Vec4 bounds) { m_bounds = bounds; Recalculate(); }
		Vec4 GetBounds() { return m_bounds; }
		
		float GetAspectRatio() { return m_aspectRatio; }
		void SetAspectRatio(float value) { m_aspectRatio = value; }
		
		Vec2 ScreenToWorld(Vec2 screenPoint);
		
		glm::mat4 GetProjection() { return m_projection; }
		glm::mat4 GetView() { return m_view; }
		glm::mat4 GetModel() { return m_model; }
		
		void SetProjectionType(EProjectionType type) {m_projectionType = type; Recalculate();}
		EProjectionType GetProjectionType() {return m_projectionType;}
		
		private:
		void Recalculate();
		
		public:
		Vec4 BackgroundColour = Vec4(0.0f, 0.0f, 0.1f, 1.0f);
		
		private:
		EProjectionType m_projectionType = EProjectionType::ORTHAGRAPHIC;
		Vec4 m_bounds = Vec4(-5.0f, 5.0f, -5.0f, 5.0f);
		float m_aspectRatio = 1.77f;
		
		glm::mat4 m_projection = glm::mat4();
		glm::mat4 m_view = glm::mat4(1.0f);
		glm::mat4 m_model = glm::mat4();
	};
}

#endif //_CAMERA_H
