/* date = November 24th 2020 8:55 am */
//SINGLETON CLASS, SHOULD ALREADY EXIST IN APPLICATION ON APPLICATION START
//This class is a layer updated on the layer stack in the main loop inside Application.cpp
//it specifically handles the mapping of "buttons" to keycodes / mousecodes / gamepad codes
//Loads in inputs from a text file, and updates the relevant input on the relevant event
//NOTE: currently does not support gamepad buttons

#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H

#include <Chaos/Core/Layer.h>
#include <Chaos/Input/KeyCodes.h>
#include <string> 
#include <map>

namespace Chaos
{
	
	struct Button
	{
		static const uint16_t MAX_KEYCODE_VALUE = std::numeric_limits<uint16_t>::max();
		static const int MAX_KEYCODES_PER_BUTTON = 4;
		
		Button() {
			for (size_t i = 0; i < MAX_KEYCODES_PER_BUTTON; ++i)
			{
				PositiveInput[i] = (KeyCode)MAX_KEYCODE_VALUE;
				NegativeInput[i] = (KeyCode)MAX_KEYCODE_VALUE;
			}
		}
		
		KeyCode PositiveInput[MAX_KEYCODES_PER_BUTTON];
		KeyCode NegativeInput[MAX_KEYCODES_PER_BUTTON];
		int PositiveInsertIndex = 0;
		int NegativeInsertIndex = 0;
		
		bool Pressed = false;
		bool Released = false;
		float ControllerValue = 0.0f;
		float KeyValue = 0.0f;
		float Value = 0.0f;
		float ValueLastFrame = 0.0f;
	};
	
	class InputManager : public Layer
	{
		public:
		InputManager() {}; //Default constructor for singleton pattern
		InputManager(const char* configFile);
		
		//from layer
		void OnAttach() override;
		void OnUpdate(float deltaTime) override; //NOTE: resets the up and down values for buttons and sets the ValueLastFrame variable in each of the buttons 
		
		void LoadInputs(const char* filePath);
		
		inline bool IsGamepadButton(KeyCode keycode);
		inline bool IsGamepadAxis(KeyCode keycode);
		inline bool IsMouseClick(KeyCode keycode);
		
		std::map<std::string, Button>& GetButtonMap() {return m_buttonMap;}
		static InputManager* Get();
		
		private:
		
		std::map<std::string, Button> m_buttonMap;
		static InputManager* s_instance;
		
		//TODO: switch all of these over to the macro inside keycodes.h that way if we want to change them, we don't have to come here and switch all of these over every time
		const std::map<std::string, KeyCode> STRING_TO_KEYCODE = {
			{"MOUSE_BUTTON_0",				MOUSE_BUTTON_0},
			{"MOUSE_BUTTON_1",				MOUSE_BUTTON_1},
			{"MOUSE_BUTTON_2",				MOUSE_BUTTON_2},
			{"MOUSE_BUTTON_3",				MOUSE_BUTTON_3},
			{"MOUSE_BUTTON_4",				MOUSE_BUTTON_4},
			{"MOUSE_BUTTON_5",				MOUSE_BUTTON_5},
			{"MOUSE_BUTTON_6",				MOUSE_BUTTON_6},
			{"MOUSE_BUTTON_7",				MOUSE_BUTTON_7},
			{"MOUSE_BUTTON_LAST",			MOUSE_BUTTON_LAST},
			{"MOUSE_BUTTON_LEFT",			MOUSE_BUTTON_LEFT},
			{"MOUSE_BUTTON_RIGHT",			MOUSE_BUTTON_RIGHT},
			{"MOUSE_BUTTON_MIDDLE",			MOUSE_BUTTON_MIDDLE},
			{"KEY_SPACE",					::Chaos::Key::Space},
			{"KEY_APOSTROPHE",				::Chaos::Key::Apostrophe},    /* ' */
			{"KEY_COMMA",					::Chaos::Key::Comma},         /* , */
			{"KEY_MINUS",					::Chaos::Key::Minus},         /* - */
			{"KEY_PERIOD",					::Chaos::Key::Period},        /* . */
			{"KEY_SLASH",					::Chaos::Key::Slash},         /* / */
			{"KEY_0",						::Chaos::Key::D0},
			{"KEY_1",						::Chaos::Key::D1},
			{"KEY_2",						::Chaos::Key::D2},
			{"KEY_3",						::Chaos::Key::D3},
			{"KEY_4",						::Chaos::Key::D4},
			{"KEY_5",						::Chaos::Key::D5},
			{"KEY_6",						::Chaos::Key::D6},
			{"KEY_7",						::Chaos::Key::D7},
			{"KEY_8",						::Chaos::Key::D8},
			{"KEY_9",						::Chaos::Key::D9},
			{"KEY_SEMICOLON",				::Chaos::Key::Semicolon},     /* ; */
			{"KEY_EQUAL",					::Chaos::Key::Equal},         /* = */
			{"KEY_A",						::Chaos::Key::A},
			{"KEY_B",						::Chaos::Key::B},
			{"KEY_C",						::Chaos::Key::C},
			{"KEY_D",						::Chaos::Key::D},
			{"KEY_E",						::Chaos::Key::E},
			{"KEY_F",						::Chaos::Key::F},
			{"KEY_G",						::Chaos::Key::G},
			{"KEY_H",						::Chaos::Key::H},
			{"KEY_I",						::Chaos::Key::I},
			{"KEY_J",						::Chaos::Key::J},
			{"KEY_K",						::Chaos::Key::K},
			{"KEY_L",						::Chaos::Key::L},
			{"KEY_M",						::Chaos::Key::M},
			{"KEY_N",						::Chaos::Key::N},
			{"KEY_O",						::Chaos::Key::O},
			{"KEY_P",						::Chaos::Key::P},
			{"KEY_Q",						::Chaos::Key::Q},
			{"KEY_R",						::Chaos::Key::R},
			{"KEY_S",						::Chaos::Key::S},
			{"KEY_T",						::Chaos::Key::T},
			{"KEY_U",						::Chaos::Key::U},
			{"KEY_V",						::Chaos::Key::V},
			{"KEY_W",						::Chaos::Key::W},
			{"KEY_X",						::Chaos::Key::X},
			{"KEY_Y",						::Chaos::Key::Y},
			{"KEY_Z",						::Chaos::Key::Z},
			{"KEY_LEFT_BRACKET",			::Chaos::Key::LeftBracket},   /* [ */
			{"KEY_BACKSLASH",				::Chaos::Key::Backslash},     /* \ */
			{"KEY_RIGHT_BRACKET",			::Chaos::Key::RightBracket},  /* ] */
			{"KEY_GRAVE_ACCENT",			::Chaos::Key::GraveAccent},   /* ` */
			{"KEY_WORLD_1",					::Chaos::Key::World1},        /* non-US #1 */
			{"KEY_WORLD_2",					::Chaos::Key::World2},        /* non-US #2 */
			
			{"KEY_ESCAPE",					::Chaos::Key::Escape},
			{"KEY_ENTER",					::Chaos::Key::Enter},
			{"KEY_TAB",						::Chaos::Key::Tab},
			{"KEY_BACKSPACE",				::Chaos::Key::Backspace},
			{"KEY_INSERT",					::Chaos::Key::Insert},
			{"KEY_DELETE",					::Chaos::Key::Delete},
			{"KEY_RIGHT",					::Chaos::Key::Right},
			{"KEY_LEFT",					::Chaos::Key::Left},
			{"KEY_DOWN",					::Chaos::Key::Down},
			{"KEY_UP",						::Chaos::Key::Up},
			{"KEY_PAGE_UP",					::Chaos::Key::PageUp},
			{"KEY_PAGE_DOWN",				::Chaos::Key::PageDown},
			{"KEY_HOME",					::Chaos::Key::Home},
			{"KEY_END",						::Chaos::Key::End},
			{"KEY_CAPS_LOCK",				::Chaos::Key::CapsLock},
			{"KEY_SCROLL_LOCK",				::Chaos::Key::ScrollLock},
			{"KEY_NUM_LOCK",				::Chaos::Key::NumLock},
			{"KEY_PRINT_SCREEN",			::Chaos::Key::PrintScreen},
			{"KEY_PAUSE",					::Chaos::Key::Pause},
			{"KEY_F1",						::Chaos::Key::F1},
			{"KEY_F2",						::Chaos::Key::F2},
			{"KEY_F3",						::Chaos::Key::F3},
			{"KEY_F4",						::Chaos::Key::F4},
			{"KEY_F5",						::Chaos::Key::F5},
			{"KEY_F6",						::Chaos::Key::F6},
			{"KEY_F7",						::Chaos::Key::F7},
			{"KEY_F8",						::Chaos::Key::F8},
			{"KEY_F9",						::Chaos::Key::F9},
			{"KEY_F10",						::Chaos::Key::F10},
			{"KEY_F11",						::Chaos::Key::F11},
			{"KEY_F12",						::Chaos::Key::F12},
			{"KEY_F13",						::Chaos::Key::F13},
			{"KEY_F14",						::Chaos::Key::F14},
			{"KEY_F15",						::Chaos::Key::F15},
			{"KEY_F16",						::Chaos::Key::F16},
			{"KEY_F17",						::Chaos::Key::F17},
			{"KEY_F18",						::Chaos::Key::F18},
			{"KEY_F19",						::Chaos::Key::F19},
			{"KEY_F20",						::Chaos::Key::F20},
			{"KEY_F21",						::Chaos::Key::F21},
			{"KEY_F22",						::Chaos::Key::F22},
			{"KEY_F23",						::Chaos::Key::F23},
			{"KEY_F24",						::Chaos::Key::F24},
			{"KEY_F25",						::Chaos::Key::F25},
			
			{"KEY_KP_0",					::Chaos::Key::KP0},
			{"KEY_KP_1",					::Chaos::Key::KP1},
			{"KEY_KP_2",					::Chaos::Key::KP2},
			{"KEY_KP_3",					::Chaos::Key::KP3},
			{"KEY_KP_4",					::Chaos::Key::KP4},
			{"KEY_KP_5",					::Chaos::Key::KP5},
			{"KEY_KP_6",					::Chaos::Key::KP6},
			{"KEY_KP_7",					::Chaos::Key::KP7},
			{"KEY_KP_8",					::Chaos::Key::KP8},
			{"KEY_KP_9",					::Chaos::Key::KP9},
			{"KEY_KP_DECIMAL",				::Chaos::Key::KPDecimal},
			{"KEY_KP_DIVIDE",				::Chaos::Key::KPDivide},
			{"KEY_KP_MULTIPLY",				::Chaos::Key::KPMultiply},
			{"KEY_KP_SUBTRACT",				::Chaos::Key::KPSubtract},
			{"KEY_KP_ADD",					::Chaos::Key::KPAdd},
			{"KEY_KP_ENTER",				::Chaos::Key::KPEnter},
			{"KEY_KP_EQUAL",				::Chaos::Key::KPEqual},
			{"KEY_LEFT_SHIFT",				::Chaos::Key::LeftShift},
			{"KEY_LEFT_CONTROL",			::Chaos::Key::LeftControl},
			{"KEY_LEFT_ALT",				::Chaos::Key::LeftAlt},
			{"KEY_LEFT_SUPER",				::Chaos::Key::LeftSuper},
			{"KEY_RIGHT_SHIFT",				::Chaos::Key::RightShift},
			{"KEY_RIGHT_CONTROL",			::Chaos::Key::RightControl},
			{"KEY_RIGHT_ALT",				::Chaos::Key::RightAlt},
			{"KEY_RIGHT_SUPER",				::Chaos::Key::RightSuper},
			{"KEY_MENU",					::Chaos::Key::Menu},
			{"GAMEPAD_BUTTON_A",			GAMEPAD_BUTTON_A},
			{"GAMEPAD_BUTTON_B",			GAMEPAD_BUTTON_B},
			{"GAMEPAD_BUTTON_X",			GAMEPAD_BUTTON_X},
			{"GAMEPAD_BUTTON_Y",			GAMEPAD_BUTTON_Y},
			{"GAMEPAD_BUTTON_LEFT_BUMPER",	GAMEPAD_BUTTON_LEFT_BUMPER},
			{"GAMEPAD_BUTTON_RIGHT_BUMPER",	GAMEPAD_BUTTON_RIGHT_BUMPER},
			{"GAMEPAD_BUTTON_BACK",			GAMEPAD_BUTTON_BACK},
			{"GAMEPAD_BUTTON_START",		GAMEPAD_BUTTON_START},
			{"GAMEPAD_BUTTON_GUIDE",		GAMEPAD_BUTTON_GUIDE},
			{"GAMEPAD_BUTTON_LEFT_THUMB",	GAMEPAD_BUTTON_LEFT_THUMB},
			{"GAMEPAD_BUTTON_RIGHT_THUMB",	GAMEPAD_BUTTON_RIGHT_THUMB},
			{"GAMEPAD_BUTTON_DPAD_UP",		GAMEPAD_BUTTON_DPAD_UP},
			{"GAMEPAD_BUTTON_DPAD_DOWN",	GAMEPAD_BUTTON_DPAD_DOWN},
			{"GAMEPAD_BUTTON_DPAD_LEFT",	GAMEPAD_BUTTON_DPAD_LEFT},
			{"GAMEPAD_BUTTON_DPAD_RIGHT",	GAMEPAD_BUTTON_DPAD_RIGHT},
			{"GAMEPAD_AXIS_LEFT_X",			GAMEPAD_AXIS_LEFT_X},
			{"GAMEPAD_AXIS_LEFT_Y",			GAMEPAD_AXIS_LEFT_Y},
			{"GAMEPAD_AXIS_RIGHT_X",		GAMEPAD_AXIS_RIGHT_X},
			{"GAMEPAD_AXIS_RIGHT_Y",		GAMEPAD_AXIS_RIGHT_Y},
			{"GAMEPAD_AXIS_LEFT_TRIGGER",	GAMEPAD_AXIS_LEFT_TRIGGER },
			{"GAMEPAD_AXIS_RIGHT_TRIGGER",	GAMEPAD_AXIS_RIGHT_TRIGGER }			
		};
		
	};
}

#endif //_INPUT_MANAGER_H
