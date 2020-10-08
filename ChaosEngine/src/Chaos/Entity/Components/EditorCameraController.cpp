#include <chaospch.h>
#include <Chaos/Entity/Components/EditorCameraController.h>
#include <Chaos/Input/Input.h>
#include <Chaos/Core/Time.h>
#include <Chaos/DataTypes/Vec2.h>
#include <Chaos/DataTypes/Vec3.h>

namespace Chaos
{
	void EditorCameraController::Update()
	{
		Vec2 dir;
		if(Input::IsKeyPressed(KEY_W))
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
		
		if(Input::IsKeyPressed(KEY_A))
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
		
		GetEntity()->GetPosition() +=  Vec3(dir.X * m_cameraSpeed * Time::GetDeltaTime(), dir.Y * m_cameraSpeed * Time::GetDeltaTime(), 0);
	}
}