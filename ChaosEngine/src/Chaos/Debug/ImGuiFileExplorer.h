/* date = October 9th 2020 5:25 pm */

#ifndef _IM_GUI_FILE_EXPLORER_H
#define _IM_GUI_FILE_EXPLORER_H

#include <Chaos/Debug/ImGuiLayer.h>
#include <filesystem>

namespace Chaos
{
	class ImGuiFileExplorer : public ImGuiLayer
	{
		public:
		ImGuiFileExplorer(char** filePath);
		
		void OnDetach() override;
		void OnImGuiUpdate() override;
		
		private:
		char** m_filePath;
		char m_tempPath[256]; //path that gets copied into the file path pointer pointer passed into the constructor when a file is selected
		
		std::filesystem::path m_rootPath;
	};
}

#endif //_IM_GUI_FILE_EXPLORER_H
