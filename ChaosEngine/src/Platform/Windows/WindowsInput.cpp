//This file should only be included on windows builds, if it is a window build, generic definitions in Input will look up / be defined here
#include "chaospch.h"
#include "Chaos/Input/Input.h"

#include "Chaos/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Chaos
{
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
	
	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}
	
	bool Input::IsMouseButtonReleased(MouseCode button)
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
}