/* date = October 8th 2020 0:40 pm */

#ifndef _CONSOLE_H
#define _CONSOLE_H
#include <functional>
#include <map>

namespace Chaos
{
	class Console
	{
		public:
		static std::unordered_map<std::string, std::function<void()>> Commands;
		static std::vector<char*> ConsoleHistory;
		
		static void ClearHistory();
		static void ParseCommand(char* command);
		static void AddCommand(std::string command, std::function<void()> function);
	};
}

#endif //_CONSOLE_H
