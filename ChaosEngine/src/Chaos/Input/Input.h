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
		inline static bool IsKeyPressed(KeyCode key) { return s_instance->IsKeyPressedImpl(key); }
		inline static bool IsKeyReleased(KeyCode key) { return s_instance->IsKeyReleasedImpl(key); }

		//MOUSE
		inline static bool IsMouseButtonPressed(MouseCode button) { return s_instance->IsMouseButtonPressedImpl(button); }
		inline static bool IsMouseButtonReleased(MouseCode button) { return s_instance->IsMouseButtonReleasedImpl(button); }
		inline static std::pair<float, float> GetMousePosition() { return s_instance->GetMousePositionImpl(); }
		inline static float GetMouseX() { return s_instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_instance->GetMouseYImpl(); }
	protected:
		//IMPLEMENTATIONS
		//KEYS
		virtual bool IsKeyPressedImpl(KeyCode key) = 0;
		virtual bool IsKeyReleasedImpl(KeyCode key) = 0;

		//MOUSE
		virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
		virtual bool IsMouseButtonReleasedImpl(MouseCode button) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;


	private:
		static Input* s_instance;
	};
}