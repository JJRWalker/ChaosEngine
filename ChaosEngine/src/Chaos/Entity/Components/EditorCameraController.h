/* date = October 8th 2020 10:41 pm */

#ifndef _EDITOR_CAMERA_CONTROLLER_H
#define _EDITOR_CAMERA_CONTROLLER_H
#include <Chaos/Entity/Component.h>

namespace Chaos
{
	class EditorCameraController : public Component
	{
		public:
		EditorCameraController(){};
		
		virtual void Update() override;
		private:
		float m_cameraSpeed = 1.0f;
	};
}

#endif //_EDITOR_CAMERA_CONTROLLER_H
