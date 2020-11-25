/* date = November 24th 2020 8:55 am */

#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H

#include <Chaos/Input/KeyCodes.h>

namespace Chaos
{
	class InputManager
	{
		public:
		InputManager(const char* configFile);
		
		std::map<const char*, KeyCode> GetButtonMap;
		
		static InputManager Get();
		
		private:
		std::map<const char*, KeyCode> m_buttonMap;
		static InputManager s_instance;
	};
}

#endif //_INPUT_MANAGER_H
