#include "chaospch.h"

#include "CameraController.h"
#include "Chaos/Entity/Components/Transform.h"
#include "Chaos/Entity/Components/Camera.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/Input/Input.h"

namespace Chaos
{
	void CameraController::Update(float deltaTime, Vec2 viewportSize, bool viewportFocused)
	{
		if (viewportFocused)
		{
			Vec2 dir;
			if (Input::IsKeyPressed(KEY_W))
			{
				dir.Y = 1;
			}
			else if (Input::IsKeyPressed(KEY_S))
			{
				dir.Y = -1;
			}
			else
			{
				dir.Y = 0;
			}

			if (Input::IsKeyPressed(KEY_A))
			{
				dir.X = -1;
			}
			else if (Input::IsKeyPressed(KEY_D))
			{
				dir.X = 1;
			}
			else
			{
				dir.X = 0;
			}
			if (Input::IsKeyPressed(KEY_E))
			{
				m_cameraTransfrom.Rotation().X += 1 * deltaTime;
			}

			m_cameraTransfrom.Position() = { m_cameraTransfrom.Position().X + (dir.X * m_moveSpeed * deltaTime), m_cameraTransfrom.Position().Y + (dir.Y * m_moveSpeed * deltaTime), 0 };
		}

		m_aspectRatio = viewportSize.X / viewportSize.Y;
		m_camera.SetAspectRatio(m_aspectRatio);
	}
	void CameraController::OnEvent(Event& e, bool viewportHovered)
	{
		EventDispatcher dispatcher(e);
		if (viewportHovered && e.GetEventType() == EventType::MouseScrolled)
		{
			dispatcher.Dispatch<MouseScrollEvent>(BIND_EVENT_FN(CameraController::OnMouseScroll));
		}
	}

	bool CameraController::OnMouseScroll(MouseScrollEvent& e)
	{
		m_zoom -= e.GetScrollY() * 0.25f;
		m_zoom = std::max(m_zoom, 0.25f);
		m_camera.SetBounds({ -m_zoom, m_zoom, -m_zoom, m_zoom });
		return false;
	}

	bool CameraController::OnKeyPress(KeyPressedEvent& e)
	{
		return false;
	}

}