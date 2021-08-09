/* 
* Taken from microsoft docs: https://docs.microsoft.com/en-us/windows/win32/learnwin32/example--the-open-dialog-box
*/

#include "chaospch.h"

#include <Chaos/Serialisation/FileUtils.h>

#include <windows.h>
#include <shobjidl.h> 

#include <sstream>

namespace FileUtils
{
	bool FileUtils::OpenFileDialog(std::string& outFilePath)
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
									COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog *pFileOpen;
			
			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
								  IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
			
			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pFileOpen->Show(NULL);
				
				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem *pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						
						// Display the file name to the user.
						if (SUCCEEDED(hr))
						{
							std::wstringstream ss;
							
							ss << pszFilePath;
							
							char temp[128];
							wcstombs(temp, pszFilePath, 128);
							
							outFilePath = std::string(temp);
							
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
		
		return SUCCEEDED(hr);
	}
	
	
	bool FileUtils::SaveFileDialog(std::string& outFilePath)
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
									COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog *pFileOpen;
			
			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, 
								  IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileOpen));
			
			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pFileOpen->Show(NULL);
				
				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem *pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						
						// Display the file name to the user.
						if (SUCCEEDED(hr))
						{
							std::wstringstream ss;
							
							ss << pszFilePath;
							
							char temp[128];
							wcstombs(temp, pszFilePath, 128);
							
							outFilePath = std::string(temp);
							
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
		
		return SUCCEEDED(hr);
	}
}