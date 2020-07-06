#pragma once
#include "Chaos/Events/MouseEvent.h"

namespace Chaos
{
	class Transform;
	class Camera;
	class Vec2;
	class CameraController
	{
	public: 
		CameraController(Transform& transform, Camera& camera) : m_cameraTransfrom(transform), m_camera(camera) {};
		void Update(float deltaTime, Vec2 viewportSize);
		void OnEvent(Event& e);
		bool OnMouseScroll(MouseScrollEvent& e);
	private:
		Transform& m_cameraTransfrom;
		Camera& m_camera;
		float m_moveSpeed = 5;
		float m_zoom = 1;
		float m_aspectRatio = 1.7f;
	};
}