#pragma once


#include "Chaos/Core/Core.h"
#include "Chaos/Input/KeyCodes.h"
#include "Chaos/Input/MouseCodes.h"
#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class Input
	{
		public:
		//STATIC DEFINITIONS
		//KEYS
		static bool IsKeyPressed(KeyCode key);
		static bool IsKeyReleased(KeyCode key);
		
		//MOUSE
		static bool IsMouseButtonPressed(KeyCode button);
		static bool IsMouseButtonReleased(KeyCode button);
		static Vec2 GetMousePosition(); //returns the viewport position as a value between -1 and 1 in x and y
		static Vec2 GetMouseDelta();
		static std::pair<float, float> GetMousePositionRaw();
		static float GetMouseX();
		static float GetMouseY();
		
		static void UpdateMouseEndFramePosition();
		
		//GAMEPAD
		static bool IsGamepadButtonPressed(KeyCode button, int controllerID = 0);
		static bool IsGamepadButtonReleased(KeyCode button, int controllerID = 0);
		
		static float GetGamepadAxis(KeyCode button, int controllerID = 0);
		
		//BUTTONS
		static float GetButton(const char* buttonName);
		static bool GetButtonDown(const char* buttonName);
		static bool GetButtonUp(const char* buttonName);
		
		public:
		static float GamepadStickDeadzone;
		
		private:
		static Vec2 m_mouseEndFramePosition;
		static Vec2 m_mouseDelta;
	};
}