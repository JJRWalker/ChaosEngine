#include <chaospch.h>
#include <Chaos/Debug/Console.h>

namespace Chaos
{
	
	void Console::ClearHistory()
	{
		ConsoleHistory.clear();
	}
	
	void Console::ParseCommand(char* command)
	{
		//store command in history
		char* toAdd = new char();
		memset(toAdd, 0, sizeof(command));
		strcpy(toAdd, command);
		ConsoleHistory.push_back(toAdd);
		
		//find command
		auto it = Commands.find(command);
		if (it != Commands.end())
		{
			it->second();
		}
		else
		{
			ConsoleHistory.push_back("Command not found!");
		}
	}
	
	void Console::AddCommand(std::string command, std::function<void()> function)
	{
		Commands.emplace(command, function);
	}
}