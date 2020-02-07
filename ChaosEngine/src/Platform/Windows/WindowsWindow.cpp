#include "chaospch.h"
#include "WindowsWindow.h"

#include "Chaos/Events/ApplicationEvent.h"
#include "Chaos/Events/MouseEvent.h"
#include "Chaos/Events/KeyEvent.h"

namespace Chaos
{
	static bool sGLFWInitialised = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LOGCORE_ERROR("GLFW ERROR ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		//INIT
		mData.Title = props.Title;
		mData.Width = props.Width;
		mData.Height = props.Height;

		LOGCORE_INFO("CREATING WINDOW {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!sGLFWInitialised)
		{
			int success = glfwInit();
			COREASSERT(success, "COULD NOT INITALISE GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			sGLFWInitialised = true;
		}
		mWindow = glfwCreateWindow((int)props.Width, (int)props.Height, mData.Title.c_str(), nullptr, nullptr);
		
		glfwSetWindowUserPointer(mWindow, &mData);
		SetVSync(true);

		//CALLBACKS
		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
				}
			}
		);
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(mWindow);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		mData.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return mData.VSync;
	}
}