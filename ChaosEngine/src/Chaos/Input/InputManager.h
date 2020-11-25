/* date = November 24th 2020 8:55 am */

#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H

#include <Chaos/Input/KeyCodes.h>
#include <string> 

namespace Chaos
{
	
	struct Button
	{
		static const int MAX_KEYCODES_PER_BUTTON = 4;
		KeyCode PositiveInput[MAX_KEYCODES_PER_BUTTON];
		KeyCode NegativeInput[MAX_KEYCODES_PER_BUTTON];
		int PositiveInsertIndex = 0;
		int NegativeInsertIndex = 0;
	};
	
	class InputManager
	{
		public:
		InputManager() {}; //Default constructor for singleton pattern
		InputManager(const char* configFile);
		
		void LoadInputs(const char* filePath);
		
		std::map<std::string, Button>& GetButtonMap() {return m_buttonMap;}
		static InputManager* Get();
		
		private:
		
		std::map<std::string, Button> m_buttonMap;
		static InputManager* s_instance;
		
		const std::map<std::string, KeyCode> STRING_TO_KEYCODE = {
			{"KEY_SPACE",           ::Chaos::Key::Space},
			{"KEY_APOSTROPHE",      ::Chaos::Key::Apostrophe},    /* ' */
			{"KEY_COMMA",           ::Chaos::Key::Comma},         /* , */
			{"KEY_MINUS",           ::Chaos::Key::Minus},         /* - */
			{"KEY_PERIOD",          ::Chaos::Key::Period},        /* . */
			{"KEY_SLASH",           ::Chaos::Key::Slash},         /* / */
			{"KEY_0",               ::Chaos::Key::D0},
			{"KEY_1",               ::Chaos::Key::D1},
			{"KEY_2",               ::Chaos::Key::D2},
			{"KEY_3",               ::Chaos::Key::D3},
			{"KEY_4",               ::Chaos::Key::D4},
			{"KEY_5",               ::Chaos::Key::D5},
			{"KEY_6",               ::Chaos::Key::D6},
			{"KEY_7",               ::Chaos::Key::D7},
			{"KEY_8",               ::Chaos::Key::D8},
			{"KEY_9",               ::Chaos::Key::D9},
			{"KEY_SEMICOLON",       ::Chaos::Key::Semicolon},     /* ; */
			{"KEY_EQUAL",           ::Chaos::Key::Equal},         /* = */
			{"KEY_A",               ::Chaos::Key::A},
			{"KEY_B",               ::Chaos::Key::B},
			{"KEY_C",               ::Chaos::Key::C},
			{"KEY_D",               ::Chaos::Key::D},
			{"KEY_E",               ::Chaos::Key::E},
			{"KEY_F",               ::Chaos::Key::F},
			{"KEY_G",               ::Chaos::Key::G},
			{"KEY_H",               ::Chaos::Key::H},
			{"KEY_I",               ::Chaos::Key::I},
			{"KEY_J",               ::Chaos::Key::J},
			{"KEY_K",               ::Chaos::Key::K},
			{"KEY_L",               ::Chaos::Key::L},
			{"KEY_M",               ::Chaos::Key::M},
			{"KEY_N",               ::Chaos::Key::N},
			{"KEY_O",               ::Chaos::Key::O},
			{"KEY_P",               ::Chaos::Key::P},
			{"KEY_Q",               ::Chaos::Key::Q},
			{"KEY_R",               ::Chaos::Key::R},
			{"KEY_S",               ::Chaos::Key::S},
			{"KEY_T",               ::Chaos::Key::T},
			{"KEY_U",               ::Chaos::Key::U},
			{"KEY_V",               ::Chaos::Key::V},
			{"KEY_W",               ::Chaos::Key::W},
			{"KEY_X",               ::Chaos::Key::X},
			{"KEY_Y",               ::Chaos::Key::Y},
			{"KEY_Z",               ::Chaos::Key::Z},
			{"KEY_LEFT_BRACKET",    ::Chaos::Key::LeftBracket},   /* [ */
			{"KEY_BACKSLASH",       ::Chaos::Key::Backslash},     /* \ */
			{"KEY_RIGHT_BRACKET",   ::Chaos::Key::RightBracket},  /* ] */
			{"KEY_GRAVE_ACCENT",    ::Chaos::Key::GraveAccent},   /* ` */
			{"KEY_WORLD_1",         ::Chaos::Key::World1},        /* non-US #1 */
			{"KEY_WORLD_2",         ::Chaos::Key::World2},        /* non-US #2 */
			
			{"KEY_ESCAPE",          ::Chaos::Key::Escape},
			{"KEY_ENTER",           ::Chaos::Key::Enter},
			{"KEY_TAB",             ::Chaos::Key::Tab},
			{"KEY_BACKSPACE",       ::Chaos::Key::Backspace},
			{"KEY_INSERT",          ::Chaos::Key::Insert},
			{"KEY_DELETE",          ::Chaos::Key::Delete},
			{"KEY_RIGHT",           ::Chaos::Key::Right},
			{"KEY_LEFT",            ::Chaos::Key::Left},
			{"KEY_DOWN",            ::Chaos::Key::Down},
			{"KEY_UP",              ::Chaos::Key::Up},
			{"KEY_PAGE_UP",         ::Chaos::Key::PageUp},
			{"KEY_PAGE_DOWN",       ::Chaos::Key::PageDown},
			{"KEY_HOME",            ::Chaos::Key::Home},
			{"KEY_END",             ::Chaos::Key::End},
			{"KEY_CAPS_LOCK",       ::Chaos::Key::CapsLock},
			{"KEY_SCROLL_LOCK",     ::Chaos::Key::ScrollLock},
			{"KEY_NUM_LOCK",        ::Chaos::Key::NumLock},
			{"KEY_PRINT_SCREEN",    ::Chaos::Key::PrintScreen},
			{"KEY_PAUSE",           ::Chaos::Key::Pause},
			{"KEY_F1",              ::Chaos::Key::F1},
			{"KEY_F2",              ::Chaos::Key::F2},
			{"KEY_F3",              ::Chaos::Key::F3},
			{"KEY_F4",              ::Chaos::Key::F4},
			{"KEY_F5",              ::Chaos::Key::F5},
			{"KEY_F6",              ::Chaos::Key::F6},
			{"KEY_F7",              ::Chaos::Key::F7},
			{"KEY_F8",              ::Chaos::Key::F8},
			{"KEY_F9",              ::Chaos::Key::F9},
			{"KEY_F10",             ::Chaos::Key::F10},
			{"KEY_F11",             ::Chaos::Key::F11},
			{"KEY_F12",             ::Chaos::Key::F12},
			{"KEY_F13",             ::Chaos::Key::F13},
			{"KEY_F14",             ::Chaos::Key::F14},
			{"KEY_F15",             ::Chaos::Key::F15},
			{"KEY_F16",             ::Chaos::Key::F16},
			{"KEY_F17",             ::Chaos::Key::F17},
			{"KEY_F18",             ::Chaos::Key::F18},
			{"KEY_F19",             ::Chaos::Key::F19},
			{"KEY_F20",             ::Chaos::Key::F20},
			{"KEY_F21",             ::Chaos::Key::F21},
			{"KEY_F22",             ::Chaos::Key::F22},
			{"KEY_F23",             ::Chaos::Key::F23},
			{"KEY_F24",             ::Chaos::Key::F24},
			{"KEY_F25",             ::Chaos::Key::F25},
			
			{"KEY_KP_0",            ::Chaos::Key::KP0},
			{"KEY_KP_1",            ::Chaos::Key::KP1},
			{"KEY_KP_2",            ::Chaos::Key::KP2},
			{"KEY_KP_3",            ::Chaos::Key::KP3},
			{"KEY_KP_4",            ::Chaos::Key::KP4},
			{"KEY_KP_5",            ::Chaos::Key::KP5},
			{"KEY_KP_6",            ::Chaos::Key::KP6},
			{"KEY_KP_7",            ::Chaos::Key::KP7},
			{"KEY_KP_8",            ::Chaos::Key::KP8},
			{"KEY_KP_9",            ::Chaos::Key::KP9},
			{"KEY_KP_DECIMAL",      ::Chaos::Key::KPDecimal},
			{"KEY_KP_DIVIDE",       ::Chaos::Key::KPDivide},
			{"KEY_KP_MULTIPLY",     ::Chaos::Key::KPMultiply},
			{"KEY_KP_SUBTRACT",     ::Chaos::Key::KPSubtract},
			{"KEY_KP_ADD",          ::Chaos::Key::KPAdd},
			{"KEY_KP_ENTER",        ::Chaos::Key::KPEnter},
			{"KEY_KP_EQUAL",        ::Chaos::Key::KPEqual},
			{"KEY_LEFT_SHIFT",      ::Chaos::Key::LeftShift},
			{"KEY_LEFT_CONTROL",    ::Chaos::Key::LeftControl},
			{"KEY_LEFT_ALT",        ::Chaos::Key::LeftAlt},
			{"KEY_LEFT_SUPER",      ::Chaos::Key::LeftSuper},
			{"KEY_RIGHT_SHIFT",     ::Chaos::Key::RightShift},
			{"KEY_RIGHT_CONTROL",   ::Chaos::Key::RightControl},
			{"KEY_RIGHT_ALT",       ::Chaos::Key::RightAlt},
			{"KEY_RIGHT_SUPER",     ::Chaos::Key::RightSuper},
			{"KEY_MENU",            ::Chaos::Key::Menu}
			
		};
		
	};
}

#endif //_INPUT_MANAGER_H
