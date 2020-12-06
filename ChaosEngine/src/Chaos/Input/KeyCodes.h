#pragma once


namespace Chaos
{
	typedef enum class KeyCode : uint16_t
	{
		//mouse codes
		//NOTE: potentially shouldn't be in here with keycodes but it makes things easier with button abstraction and there's no overlap with codes yet
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
		ButtonMiddle = Button2,
		
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */
		
		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */
		
		Semicolon = 59, /* ; */
		Equal = 61, /* = */
		
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		
		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */
		
		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */
		
		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,
		
		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,
		
		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;
	
	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

//MouseCode defines
//from glfw.h
#define MOUSE_BUTTON_0      ::Chaos::Key::Button0
#define MOUSE_BUTTON_1      ::Chaos::Key::Button1
#define MOUSE_BUTTON_2      ::Chaos::Key::Button2
#define MOUSE_BUTTON_3      ::Chaos::Key::Button3
#define MOUSE_BUTTON_4      ::Chaos::Key::Button4
#define MOUSE_BUTTON_5      ::Chaos::Key::Button5
#define MOUSE_BUTTON_6      ::Chaos::Key::Button6
#define MOUSE_BUTTON_7      ::Chaos::Key::Button7
#define MOUSE_BUTTON_LAST   ::Chaos::Key::ButtonLast
#define MOUSE_BUTTON_LEFT   ::Chaos::Key::ButtonLeft
#define MOUSE_BUTTON_RIGHT  ::Chaos::Key::ButtonRight
#define MOUSE_BUTTON_MIDDLE ::Chaos::Key::ButtonMiddle

//Keycode defines
// From glfw3.h
#define KEY_SPACE           ::Chaos::Key::Space
#define KEY_APOSTROPHE      ::Chaos::Key::Apostrophe    /* ' */
#define KEY_COMMA           ::Chaos::Key::Comma         /* , */
#define KEY_MINUS           ::Chaos::Key::Minus         /* - */
#define KEY_PERIOD          ::Chaos::Key::Period        /* . */
#define KEY_SLASH           ::Chaos::Key::Slash         /* / */
#define KEY_0               ::Chaos::Key::D0
#define KEY_1               ::Chaos::Key::D1
#define KEY_2               ::Chaos::Key::D2
#define KEY_3               ::Chaos::Key::D3
#define KEY_4               ::Chaos::Key::D4
#define KEY_5               ::Chaos::Key::D5
#define KEY_6               ::Chaos::Key::D6
#define KEY_7               ::Chaos::Key::D7
#define KEY_8               ::Chaos::Key::D8
#define KEY_9               ::Chaos::Key::D9
#define KEY_SEMICOLON       ::Chaos::Key::Semicolon     /* ; */
#define KEY_EQUAL           ::Chaos::Key::Equal         /* = */
#define KEY_A               ::Chaos::Key::A
#define KEY_B               ::Chaos::Key::B
#define KEY_C               ::Chaos::Key::C
#define KEY_D               ::Chaos::Key::D
#define KEY_E               ::Chaos::Key::E
#define KEY_F               ::Chaos::Key::F
#define KEY_G               ::Chaos::Key::G
#define KEY_H               ::Chaos::Key::H
#define KEY_I               ::Chaos::Key::I
#define KEY_J               ::Chaos::Key::J
#define KEY_K               ::Chaos::Key::K
#define KEY_L               ::Chaos::Key::L
//#define KEY_M              ::Chaos::Key::M
#define KEY_N               ::Chaos::Key::N
#define KEY_O               ::Chaos::Key::O
#define KEY_P               ::Chaos::Key::P
#define KEY_Q               ::Chaos::Key::Q
#define KEY_R               ::Chaos::Key::R
#define KEY_S               ::Chaos::Key::S
#define KEY_T               ::Chaos::Key::T
#define KEY_U               ::Chaos::Key::U
#define KEY_V               ::Chaos::Key::V
#define KEY_W               ::Chaos::Key::W
#define KEY_X               ::Chaos::Key::X
#define KEY_Y               ::Chaos::Key::Y
#define KEY_Z               ::Chaos::Key::Z
#define KEY_LEFT_BRACKET    ::Chaos::Key::LeftBracket   /* [ */
#define KEY_BACKSLASH       ::Chaos::Key::Backslash     /* \ */
#define KEY_RIGHT_BRACKET   ::Chaos::Key::RightBracket  /* ] */
#define KEY_GRAVE_ACCENT    ::Chaos::Key::GraveAccent   /* ` */
#define KEY_WORLD_1         ::Chaos::Key::World1        /* non-US #1 */
#define KEY_WORLD_2         ::Chaos::Key::World2        /* non-US #2 */

/* Function keys */
#define KEY_ESCAPE          ::Chaos::Key::Escape
#define KEY_ENTER           ::Chaos::Key::Enter
#define KEY_TAB             ::Chaos::Key::Tab
#define KEY_BACKSPACE       ::Chaos::Key::Backspace
#define KEY_INSERT          ::Chaos::Key::Insert
#define KEY_DELETE          ::Chaos::Key::Delete
#define KEY_RIGHT           ::Chaos::Key::Right
#define KEY_LEFT            ::Chaos::Key::Left
#define KEY_DOWN            ::Chaos::Key::Down
#define KEY_UP              ::Chaos::Key::Up
#define KEY_PAGE_UP         ::Chaos::Key::PageUp
#define KEY_PAGE_DOWN       ::Chaos::Key::PageDown
#define KEY_HOME            ::Chaos::Key::Home
#define KEY_END             ::Chaos::Key::End
#define KEY_CAPS_LOCK       ::Chaos::Key::CapsLock
#define KEY_SCROLL_LOCK     ::Chaos::Key::ScrollLock
#define KEY_NUM_LOCK        ::Chaos::Key::NumLock
#define KEY_PRINT_SCREEN    ::Chaos::Key::PrintScreen
#define KEY_PAUSE           ::Chaos::Key::Pause
#define KEY_F1              ::Chaos::Key::F1
#define KEY_F2              ::Chaos::Key::F2
#define KEY_F3              ::Chaos::Key::F3
#define KEY_F4              ::Chaos::Key::F4
#define KEY_F5              ::Chaos::Key::F5
#define KEY_F6              ::Chaos::Key::F6
#define KEY_F7              ::Chaos::Key::F7
#define KEY_F8              ::Chaos::Key::F8
#define KEY_F9              ::Chaos::Key::F9
#define KEY_F10             ::Chaos::Key::F10
#define KEY_F11             ::Chaos::Key::F11
#define KEY_F12             ::Chaos::Key::F12
#define KEY_F13             ::Chaos::Key::F13
#define KEY_F14             ::Chaos::Key::F14
#define KEY_F15             ::Chaos::Key::F15
#define KEY_F16             ::Chaos::Key::F16
#define KEY_F17             ::Chaos::Key::F17
#define KEY_F18             ::Chaos::Key::F18
#define KEY_F19             ::Chaos::Key::F19
#define KEY_F20             ::Chaos::Key::F20
#define KEY_F21             ::Chaos::Key::F21
#define KEY_F22             ::Chaos::Key::F22
#define KEY_F23             ::Chaos::Key::F23
#define KEY_F24             ::Chaos::Key::F24
#define KEY_F25             ::Chaos::Key::F25

/* Keypad */
#define KEY_KP_0            ::Chaos::Key::KP0
#define KEY_KP_1            ::Chaos::Key::KP1
#define KEY_KP_2            ::Chaos::Key::KP2
#define KEY_KP_3            ::Chaos::Key::KP3
#define KEY_KP_4            ::Chaos::Key::KP4
#define KEY_KP_5            ::Chaos::Key::KP5
#define KEY_KP_6            ::Chaos::Key::KP6
#define KEY_KP_7            ::Chaos::Key::KP7
#define KEY_KP_8            ::Chaos::Key::KP8
#define KEY_KP_9            ::Chaos::Key::KP9
#define KEY_KP_DECIMAL      ::Chaos::Key::KPDecimal
#define KEY_KP_DIVIDE       ::Chaos::Key::KPDivide
#define KEY_KP_MULTIPLY     ::Chaos::Key::KPMultiply
#define KEY_KP_SUBTRACT     ::Chaos::Key::KPSubtract
#define KEY_KP_ADD          ::Chaos::Key::KPAdd
#define KEY_KP_ENTER        ::Chaos::Key::KPEnter
#define KEY_KP_EQUAL        ::Chaos::Key::KPEqual

#define KEY_LEFT_SHIFT      ::Chaos::Key::LeftShift
#define KEY_LEFT_CONTROL    ::Chaos::Key::LeftControl
#define KEY_LEFT_ALT        ::Chaos::Key::LeftAlt
#define KEY_LEFT_SUPER      ::Chaos::Key::LeftSuper
#define KEY_RIGHT_SHIFT     ::Chaos::Key::RightShift
#define KEY_RIGHT_CONTROL   ::Chaos::Key::RightControl
#define KEY_RIGHT_ALT       ::Chaos::Key::RightAlt
#define KEY_RIGHT_SUPER     ::Chaos::Key::RightSuper
#define KEY_MENU            ::Chaos::Key::Menu
