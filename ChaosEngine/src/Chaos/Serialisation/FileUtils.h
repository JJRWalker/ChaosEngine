/* date = April 27th 2021 8:05 am */

#ifndef _FILE_UTILS_H
#define _FILE_UTILS_H

#include <string>

namespace FileUtils
{
	std::string GetFileName(std::string filePath, bool includeExtension = true);
	void RemoveExtension(std::string& path);
}

#endif //_FILE_UTILS_H
