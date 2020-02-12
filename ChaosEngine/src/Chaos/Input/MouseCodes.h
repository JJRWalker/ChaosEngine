#pragma once

namespace Chaos
{
	typedef enum class MouseCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}

#define HZ_MOUSE_BUTTON_0      ::Chaos::Mouse::Button0
#define HZ_MOUSE_BUTTON_1      ::Chaos::Mouse::Button1
#define HZ_MOUSE_BUTTON_2      ::Chaos::Mouse::Button2
#define HZ_MOUSE_BUTTON_3      ::Chaos::Mouse::Button3
#define HZ_MOUSE_BUTTON_4      ::Chaos::Mouse::Button4
#define HZ_MOUSE_BUTTON_5      ::Chaos::Mouse::Button5
#define HZ_MOUSE_BUTTON_6      ::Chaos::Mouse::Button6
#define HZ_MOUSE_BUTTON_7      ::Chaos::Mouse::Button7
#define HZ_MOUSE_BUTTON_LAST   ::Chaos::Mouse::ButtonLast
#define HZ_MOUSE_BUTTON_LEFT   ::Chaos::Mouse::ButtonLeft
#define HZ_MOUSE_BUTTON_RIGHT  ::Chaos::Mouse::ButtonRight
#define HZ_MOUSE_BUTTON_MIDDLE ::Chaos::Mouse::ButtonMiddle
