#include "chaospch.h"
#include <Chaos/Input/InputManager.h>
#include <Chaos/Events/KeyEvent.h>
#include <Chaos/Events/MouseEvent.h>
#include <Chaos/Serialisation/TextFileReader.h>

namespace Chaos
{
	InputManager* InputManager::s_instance;
	
	InputManager::InputManager(const char* configFile)
	{
		LoadInputs(configFile);
		s_instance = this;
	}
	
	void InputManager::OnAttach()
	{
		
	}
	
	//On an event if it is a desired input event, then set the values for that button if it is found.
	
	//This seems kinda silly but it's better than doing all this every frame regardless, and doing this on the input class didn't allow for features like "ButtonDown"
	void InputManager::OnEvent(Event& event)
	{
		//MOUSE INPUTS
		if (event.GetEventType() == EventType::MouseButtonPressed || event.GetEventType() == EventType::MouseButtonReleased)
		{
			MouseButtonEvent* mouseEvent = dynamic_cast<MouseButtonEvent*>(&event);
			
			std::map<std::string, Button>::iterator it;
			
			//iterate over the button map to search for the button code from the event
			for (it = m_buttonMap.begin(); it != m_buttonMap.end(); it++)
			{
				bool positive = false;
				bool negative = false;
				bool found = false;
				
				for(int i = 0; i < it->second.PositiveInsertIndex; ++ i)
				{
					if (it->second.PositiveInput[i] == (KeyCode)mouseEvent->GetMouseButton())
					{
						found = true;
						if (event.GetEventType() == EventType::MouseButtonPressed)
						{
							positive = true;
						}
					}
				}
				
				for(int i = 0; i < it->second.NegativeInsertIndex; ++ i)
				{
					if (it->second.NegativeInput[i] == (KeyCode)mouseEvent->GetMouseButton())
					{
						found = true;
						if (event.GetEventType() == EventType::MouseButtonPressed)
						{
							negative = true;
						}
					}
				}
				
				//only modify the values if the event code matches (it was found)
				if (found)
				{
					if (positive)
						it->second.Value = 1.0f;
					if (negative)
						it->second.Value = -1.0f;
					if (positive == negative)
						it->second.Value = 0.0f;
				}
			}
			
		}
		//KEY INPUTS
		//do the same for key inputs
		else if (event.GetEventType() == EventType::KeyPressed ||
				 event.GetEventType() == EventType::KeyReleased)
		{
			KeyEvent* keyEvent = dynamic_cast<KeyEvent*>(&event);
			
			std::map<std::string, Button>::iterator it;
			
			for (it = m_buttonMap.begin(); it != m_buttonMap.end(); it++)
			{
				bool positive = false;
				bool negative = false;
				bool found = false;
				
				for(int i = 0; i < it->second.PositiveInsertIndex; ++ i)
				{
					if (it->second.PositiveInput[i] == (KeyCode)keyEvent->GetKeyCode())
					{
						found = true;
						if (event.GetEventType() == EventType::KeyPressed)
						{
							positive = true;
						}
					}
				}
				
				for(int i = 0; i < it->second.NegativeInsertIndex; ++ i)
				{
					if (it->second.NegativeInput[i] == (KeyCode)keyEvent->GetKeyCode())
					{
						found = true;
						if (event.GetEventType() == EventType::KeyPressed)
						{
							negative = true;
						}
					}
				}
				
				if(found)
				{
					if (positive)
						it->second.Value = 1.0f;
					if (negative)
						it->second.Value = -1.0f;
					if (positive == negative)
						it->second.Value = 0.0f;
				}
			}
		}
		//TODO: add controller inputs here when controller support is added. Need to set up some events for controller inputs before that.
	}
	
	//sets the Pressed and Released variables depending on if the button's value changed this frame. Called from Application.cpp during the layer stack iteration 
	void InputManager::OnUpdate(float deltaTime)
	{
		std::map<std::string, Button>::iterator it;
		
		for (it = m_buttonMap.begin(); it != m_buttonMap.end(); it++)
		{
			if(it->second.Value != it->second.ValueLastFrame)
			{
				if (abs(it->second.Value) < abs(it->second.ValueLastFrame))
					it->second.Released = true;
				else
					it->second.Pressed = true;
			}
			else
			{
				it->second.Pressed = false;
				it->second.Released = false;
			}
			
			it->second.ValueLastFrame = it->second.Value;
		}
	}
	
	//Opens a specified file and reads inputs based on an operator, name of button and then a string key value
	/*E.g
+horisontal KEY_D
-horisontal KEY_A

above will essentially set up an axis for the button horisontal that will return either -1, 0 or 1 depending on the button pressed
*/
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