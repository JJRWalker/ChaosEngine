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
		static bool IsMouseButtonPressed(MouseCode button);
		static bool IsMouseButtonReleased(MouseCode button);
		static Vec2 GetMousePosition(); //returns the viewport position as a value between -1 and 1 in x and y
		static std::pair<float, float> GetMousePositionRaw();
		static float GetMouseX();
		static float GetMouseY();
	};
}