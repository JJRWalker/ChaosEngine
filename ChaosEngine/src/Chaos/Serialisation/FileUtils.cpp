#include "chaospch.h"

#include "FileUtils.h"
#include <algorithm>

namespace FileUtils
{
	std::string GetFileName(std::string filePath, bool includeExtension)
	{
		std::string name = "";
		
		if (filePath.size() == 0)
			return name;
		
		for (int i = filePath.size() - 1; i >= 0; --i)
		{
			if (filePath[i] == '\\' || filePath[i] == '/')
			{
				break;
			}
			
			name += filePath[i];
		}
		
		std::reverse(name.begin(), name.end());
		
		if (!includeExtension)
			RemoveExtension(name);
		
		return name;
	}
	
	
	void FileUtils::RemoveExtension(std::string& path)
	{
		if (path.size() == 0)
			return;
		
		size_t newSize = path.size();
		
		for (int i = path.size() - 1; i >= 0; --i)
		{
			--newSize;
			if(path[i] == '.')
			{
				path.resize(newSize);
				return;
			}
		}
	}
}