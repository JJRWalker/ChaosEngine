#include "chaospch.h"

#include "EditorCameraController.h"

#include "Chaos/Input/Input.h"
#include "Chaos/Core/Time.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec3.h"

namespace Chaos
{
	void EditorCameraController::Update(float delta)
	{
		if(Input::IsKeyPressed(KEY_W))
		{
			m_dir.Y = 1;
		}
		else if (Input::IsKeyPressed(KEY_S))
		{
			m_dir.Y = -1;
		}
		else
		{
			m_dir.Y = 0;
		}
		
		if(Input::IsKeyPressed(KEY_A))
		{
			m_dir.X = -1;
		}
		else if (Input::IsKeyPressed(KEY_D))
		{
			m_dir.X = 1;
		}
		else
		{
			m_dir.X = 0;
		}
		
		Vec2 position = GetPosition() + (m_dir * (m_cameraSpeed * Time::GetDeltaTime()));
		
		if (p_parent)
			p_parent->SetPosition(position);
	}
}