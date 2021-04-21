//This file should only be included on windows builds, if it is a window build, generic definitions in Input will look up / be defined here
#include "chaospch.h"
#include "Chaos/Input/Input.h"
#include "Chaos/Input/InputManager.h"
#include "Chaos/Core/Application.h"
#include <GLFW/glfw3.h>


namespace Chaos
{
	Vec2 Input::m_mouseEndFramePosition = Vec2(0,0);
	Vec2 Input::m_mouseDelta = Vec2(0,0);
	
	bool Input::IsKeyPressed(KeyCode key)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	
	bool Input::IsKeyReleased(KeyCode key)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_RELEASE;
	}
	
	bool Input::IsMouseButtonPressed(KeyCode button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}
	
	bool Input::IsMouseButtonReleased(KeyCode button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_RELEASE;
	}
	
	//Returns the viewport position of the mouse with x and y values between -1 and 1
	Vec2 Input::GetMousePosition()
	{
		return Vec2((GetMouseX() / Application::Get().GetWindow().GetWidth()) * 2 - 1, (GetMouseY() / Application::Get().GetWindow().GetHeight()) * 2 - 1);
	}
	
	//returns the difference between the mouse position at the end of the last frame and the current mouse position
	Vec2 Input::GetMouseDelta()
	{
		return m_mouseDelta;
	}
	
	std::pair<float, float> Input::GetMousePositionRaw()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		return { (float)xPos, (float)yPos };
	}
	
	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePositionRaw();
		return (float)x;
	}
	
	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePositionRaw();
		return (float)y;
	}
	
	void Input::UpdateMouseEndFramePosition()
	{
		m_mouseDelta = GetMousePosition() - m_mouseEndFramePosition;
		m_mouseEndFramePosition = GetMousePosition();
	}
	
	float Input::GetButton(const char* buttonName)
	{
		std::map<std::string, Button>::iterator it = InputManager::Get()->GetButtonMap().find(buttonName);
		
		bool positive = false;
		bool negative = false;
		
		if (it != InputManager::Get()->GetButtonMap().end())
		{
			for(int i = 0; i < it->second.PositiveInsertIndex; ++i)
			{
				if((uint16_t)it->second.PositiveInput[i] < Button::MAX_KEYCODE_VALUE)
				{
					auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
					//if it's a mouse code
					if ((uint16_t)it->second.PositiveInput[i] < 8)
					{
						auto state = glfwGetMouseButton(window, static_cast<int32_t>(it->second.PositiveInput[i]));
						if (state == GLFW_PRESS || state == GLFW_REPEAT)
						{
							positive = true;
						}
					}
					else
					{
						auto state = glfwGetKey(window, static_cast<int32_t>(it->second.PositiveInput[i]));
						if (state == GLFW_PRESS || state == GLFW_REPEAT)
						{
							positive = true;
						}
					}
				}
			}
			
			for(int i = 0; i < it->second.NegativeInsertIndex; ++i)
			{
				if((uint16_t)it->second.NegativeInput[i] < Button::MAX_KEYCODE_VALUE)
				{
					auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
					//if it's a mouse code
					if ((uint16_t)it->second.NegativeInput[i] < 8)
					{
						auto state = glfwGetMouseButton(window, static_cast<int32_t>(it->second.NegativeInput[i]));
						if (state == GLFW_PRESS || state == GLFW_REPEAT)
						{
							negative = true;
						}
					}
					else
					{
						auto state = glfwGetKey(window, static_cast<int32_t>(it->second.NegativeInput[i]));
						if (state == GLFW_PRESS || state == GLFW_REPEAT)
						{
							negative = true;
						}
					}
				}
			}
		}
		
		return positive - negative;
	}
	
	bool Input::GetButtonDown(const char* buttonName)
	{
		std::map<std::string, Button>::iterator it = InputManager::Get()->GetButtonMap().find(buttonName);
		
		return it->second.Pressed;
	}
	
	bool Input::GetButtonUp(const char* buttonName)
	{
		std::map<std::string, Button>::iterator it = InputManager::Get()->GetButtonMap().find(buttonName);
		
		return it->second.Released;
	}
}