/* date = February 28th 2021 0:17 pm */

#ifndef _EDITOR_CAMERA_CONTROLLER_H
#define _EDITOR_CAMERA_CONTROLLER_H

#include "Chaos/Nodes/Node.h"

namespace Chaos
{
	class EditorCameraController : public Node
	{
		public:
		EditorCameraController(bool child = false) : Node(child) { Name = "EditorCameraController"; }
		
		virtual void OnUpdate(float delta) override;
		private:
		float m_cameraSpeed = 10.0f;
		Vec2 m_dir;
	};
}

#endif //_EDITOR_CAMERA_CONTROLLER_H
