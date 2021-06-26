#include "chaospch.h"
#include <Chaos/Serialisation/TextFileReader.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace Chaos
{
	std::string TextFileReader::SelectWhere(const char* path, const char* key)
	{
		std::ifstream file(path);
		
		if(!file)
		{
			LOGCORE_WARN("TEXTFILESYSTEM: FILE NOT FOUND AT SPECIFIED PATH!");
			return "";
		}
		
		std::string line;
		
		while(std::getline(file, line))
		{
			
			std::string lineKey;
			
			for(int i = 0; i < line.length(); ++i)
			{
				if (!isspace(line[i]))
				{
					lineKey += line[i];
				}
				else
				{
					if(lineKey.compare(key) == 0)
					{
						std::string value;
						for (int j = i + 1; j < line.length(); ++j)
						{
							if (!isspace(line[j]))
							{
								value += line[j];
							}
							else
							{
								return value.c_str();
							}
						}
					}
					lineKey = "";
					break;
				}
			}
		}
		
		return "";
	}
	
	std::vector<std::pair<std::string, std::string>> TextFileReader::GetKeyValuePairs(const char* path)
	{
		std::vector<std::pair<std::string, std::string>> keyValuePairs;
		
		std::ifstream file(path);
		
		if(!file)
		{
			LOGCORE_WARN("TEXTFILESYSTEM: FILE NOT FOUND AT SPECIFIED PATH!");
			return keyValuePairs;
		}
		
		std::string line;
		
		while(std::getline(file, line))
		{
			
			std::string lineKey;
			
			for(int i = 0; i < line.length(); ++i)
			{
				if (!isspace(line[i]))
				{
					lineKey += line[i];
				}
				else
				{
					std::string value;
					for (int j = i + 1; j <= line.length(); ++j)
					{
						if (!isspace(line[j]) && j != line.length())
						{
							value += line[j];
						}
						else
						{
							keyValuePairs.push_back(std::make_pair(lineKey, value));
						}
					}
					
					lineKey = "";
				}
			}
		}
		return keyValuePairs;
	}
	
}