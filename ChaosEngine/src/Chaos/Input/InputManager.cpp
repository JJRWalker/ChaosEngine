#include "chaospch.h"
#include <Chaos/Input/InputManager.h>
#include <Chaos/Input/Input.h>
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
	
	
	//sets the Pressed and Released variables depending on if the button's value changed this frame. Called from Application.cpp during the layer stack iteration 
	void InputManager::OnUpdate(float deltaTime)
	{
		// UPDATE BASED ON KEY EVENTS
		std::map<std::string, Button>::iterator it;
		
		// update controller values while we're itterating
		// NOTE: cannot be updated using events, glfw does not support controller events
		
		for (it = m_buttonMap.begin(); it != m_buttonMap.end(); it++)
		{
			it->second.ControllerValue = 0.0f;
			it->second.KeyValue = 0.0f;
			
			for (int i = 0; i < it->second.PositiveInsertIndex; ++i)
			{
				if (IsGamepadButton(it->second.PositiveInput[i]))
				{
					it->second.ControllerValue += (float)Input::IsGamepadButtonPressed(it->second.PositiveInput[i]);
				}
				
				else if (IsGamepadAxis(it->second.PositiveInput[i]))
				{
					it->second.ControllerValue += Input::GetGamepadAxis(it->second.PositiveInput[i]);
				}
				else if (IsMouseClick(it->second.PositiveInput[i]))
				{
					it->second.KeyValue += Input::IsMouseButtonPressed(it->second.PositiveInput[i]);
				}
				else
				{
					it->second.KeyValue += Input::IsKeyPressed(it->second.PositiveInput[i]);
				}
			}
			
			for (int i = 0; i < it->second.NegativeInsertIndex; ++i)
			{
				if (IsGamepadButton(it->second.NegativeInput[i]))
				{
					it->second.ControllerValue -= (float)Input::IsGamepadButtonPressed(it->second.NegativeInput[i]);
				}
				
				else if (IsGamepadAxis(it->second.NegativeInput[i]))
				{
					it->second.ControllerValue -= Input::GetGamepadAxis(it->second.NegativeInput[i]);
				}
				else if (IsMouseClick(it->second.PositiveInput[i]))
				{
					it->second.KeyValue -= Input::IsMouseButtonPressed(it->second.NegativeInput[i]);
				}
				else
				{
					it->second.KeyValue -= Input::IsKeyPressed(it->second.NegativeInput[i]);
				}
			}
			it->second.Value = CLAMP(it->second.KeyValue + it->second.ControllerValue, -1.0f, 1.0f);
			
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
	+horizontal KEY_D
	-horizontal KEY_A
	
	above will essentially set up an axis for the button horisontal that will return either -1, 0 or 1 depending on the button pressed
	*/
	void InputManager::LoadInputs(const char* filePath)
	{
		//get our raw string map of button name to string keycode
		std::vector<std::pair<std::string, std::string>> rawInputMap = TextFileReader::GetKeyValuePairs(filePath);
		
		std::vector<std::pair<std::string, std::string>>::iterator it;
		
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
			for (int i = 0; i < testIt->second.PositiveInsertIndex; ++i)
			{
				LOGCORE_INFO("{0}: +: {1} -: {2}", testIt->first, testIt->second.PositiveInput[i], testIt->second.NegativeInput[0]);
			}
		}
		
	}
	
	
	bool InputManager::IsGamepadButton(KeyCode keycode)
	{
		return (uint16_t)keycode >= (uint16_t)GAMEPAD_BUTTON_A && (uint16_t)keycode <= (uint16_t)GAMEPAD_BUTTON_LAST;
	}
	
	
	bool InputManager::IsGamepadAxis(KeyCode keycode)
	{
		return (uint16_t)keycode >= (uint16_t)GAMEPAD_AXIS_LEFT_X  && (uint16_t)keycode <= (uint16_t)GAMEPAD_AXIS_LAST;
	}
	
	
	bool InputManager::IsMouseClick(KeyCode keycode)
	{
		return (uint16_t)keycode <= (uint16_t)MOUSE_BUTTON_LAST;
	}
	
	
	InputManager* InputManager::Get()
	{
		if(!s_instance)
			s_instance = new InputManager("./Assets/Config/Inputs.ini");
		
		return s_instance;
	}
}