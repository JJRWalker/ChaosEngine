#pragma once


#include "Chaos/Core/Core.h"
#include "Chaos/Input/KeyCodes.h"
#include "Chaos/Input/MouseCodes.h"

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
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}