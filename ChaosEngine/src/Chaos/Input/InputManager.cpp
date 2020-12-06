#include "chaospch.h"
#include <Chaos/Input/InputManager.h>
#include <Chaos/Serialisation/TextFileReader.h>

namespace Chaos
{
	InputManager* InputManager::s_instance;
	
	InputManager::InputManager(const char* configFile)
	{
		LoadInputs(configFile);
	}
	
	void InputManager::LoadInputs(const char* filePath)
	{
		//get our raw string map of button name to string keycode
		std::map<std::string, std::string> rawInputMap = TextFileReader::GetKeyValuePairs(filePath);
		
		std::map<std::string, std::string>::iterator it;
		
		//itterate over the pairs found with the text file reader
		for ( it = rawInputMap.begin(); it != rawInputMap.end(); it++)
		{
			//POSITIVE INPUTS
			if (it->first[0] == '+')
			{
				std::string key = it->first; 
				key.erase(0,1);//drop the first char as we don't want it anymore
				std::map<std::string, Button>::iterator foundButton = m_buttonMap.find(key); //check for the button map
				
				//if the button mapping already exists
				if (foundButton != m_buttonMap.end())
				{
					//only add if there is space in the array for it
					if(foundButton->second.PositiveInsertIndex < Button::MAX_KEYCODES_PER_BUTTON)
					{
						//make sure the keycode exists on our keycode map
						std::map<const std::string, KeyCode>::const_iterator codeIt = STRING_TO_KEYCODE.find(it->second);
						if (codeIt != STRING_TO_KEYCODE.end())
						{
							foundButton->second.PositiveInput[foundButton->second.PositiveInsertIndex] = codeIt->second;
							foundButton->second.PositiveInsertIndex++;
						}
						else 
							LOGCORE_WARN("INPUTMANAGER: Keycode {0} not found on keycode map", it->second);
					}
					else
					{
						LOGCORE_WARN("INPUTMANAGER: button has surpassed the max number of positive inputs cannot add another keycode to the array of inputs");
					}
				}
				else
				{
					//if there isn't an entry with that button name, make a new one with the keycode (if it exists in the map)
					Button newButton;
					std::map<const std::string, KeyCode>::const_iterator codeIt = STRING_TO_KEYCODE.find(it->second);
					if (codeIt != STRING_TO_KEYCODE.end())
					{
						newButton.PositiveInsertIndex = 1;
						newButton.PositiveInput[0] = STRING_TO_KEYCODE.find(it->second)->second;
						m_buttonMap.insert(std::make_pair(key, newButton));
					}
					else
						LOGCORE_WARN("INPUTMANAGER: Keycode {0} not found on keycode map", it->second);
				}
				
				//LOGCORE_TRACE("Positive axis for {0} assigned", key);
			}
			
			//NEGATIVE INPUTS
			else if (it->first[0] == '-')
			{
				std::string key = it->first; 
				key.erase(0,1);//drop the first char as we don't want it anymore
				std::map<std::string, Button>::iterator foundButton = m_buttonMap.find(key); //check for the button map
				
				//if the button mapping already exists
				if (foundButton != m_buttonMap.end())
				{
					//only add if there is space in the array for it
					if(foundButton->second.NegativeInsertIndex < Button::MAX_KEYCODES_PER_BUTTON)
					{
						std::map<const std::string, KeyCode>::const_iterator codeIt = STRING_TO_KEYCODE.find(it->second);
						if (codeIt != STRING_TO_KEYCODE.end())
						{
							foundButton->second.NegativeInput[foundButton->second.NegativeInsertIndex] = codeIt->second;
							foundButton->second.NegativeInsertIndex++;
						}
						else 
							LOGCORE_WARN("INPUTMANAGER: Keycode {0} not found on keycode map", it->second);
						
					}
					else
					{
						LOGCORE_WARN("INPUTMANAGER: button has surpassed the max number of negative inputs cannot add another keycode to the array of inputs");
					}
				}
				else
				{
					//if there isn't an entry with that button name, make a new one with the keycode (if it exists in the map)
					Button newButton;
					std::map<const std::string, KeyCode>::const_iterator codeIt = STRING_TO_KEYCODE.find(it->second);
					if (codeIt != STRING_TO_KEYCODE.end())
					{
						newButton.NegativeInsertIndex = 1;
						newButton.NegativeInput[0] = STRING_TO_KEYCODE.find(it->second)->second;
						m_buttonMap.insert(std::make_pair(key, newButton));
					}
					else
						LOGCORE_WARN("INPUTMANAGER: Keycode {0} not found on keycode map", it->second);
				}
			}
			else
			{
				LOGCORE_WARN("INPUTMANAGER: input {0} does not start with a '+' or a '-' and therefore a keycode cannot be determined!");
			}
		}
		
		std::map<std::string, Button>::iterator testIt;
		
		//Debug TODO: remove this once / if there is a graphical interface for this 
		LOGCORE_INFO("===INPUTS LOADED===");
		
		for (testIt = m_buttonMap.begin(); testIt != m_buttonMap.end(); testIt++)
		{
			LOGCORE_INFO("{0}: +: {1} -: {2}", testIt->first, testIt->second.PositiveInput[0], testIt->second.NegativeInput[0]);
		}
		
	}
	
	InputManager* InputManager::Get()
	{
		if(!s_instance)
			s_instance = new InputManager("./Assets/Config/Inputs.ini");
		
		return s_instance;
	}
}