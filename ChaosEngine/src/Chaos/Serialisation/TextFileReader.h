/* date = November 23rd 2020 7:36 pm */

#ifndef _TEXT_FILE_READER_H
#define _TEXT_FILE_READER_H
#include <string>
#include <map>
#include <vector>

namespace Chaos
{
	class TextFileReader
	{
		public:
		//Gets the string value of a text entry in a text file
		//Searches line by line using the first word before a space as a key, then returns the first string before a space after that key
		static std::string SelectWhere(const char* path, const char* key);
		static std::vector<std::pair<std::string, std::string>> GetKeyValuePairs(const char* path);
	};
}


#endif //_TEXT_FILE_READER_H
