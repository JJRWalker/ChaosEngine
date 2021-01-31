
//Unity build file for easy compilation outside of visual studio
#include <chaospch.h>
#include <Chaos.h>

//===CORE=======================================
#include <Chaos/Core/Application.cpp>
#include <Chaos/Core/LayerStack.cpp>
#include <Chaos/Core/Log.cpp>
#include <Chaos/Core/Level.cpp>
#include <Chaos/Core/Time.cpp>

//===DATA TYPES================================
//#include <Chaos/DataTypes/Ray2D.cpp>

//===DEBUG======================================
#include <Chaos/Debug/Console.cpp>
#include <Chaos/Debug/ImGuiBuild.cpp>
#include <Chaos/Debug/ImGuiLayer.cpp>
#include <Chaos/Debug/ImGuiConsole.cpp>
//#include <Chaos/Debug/ImGuiEditor.cpp>
#include <Chaos/Debug/ImGuiFileExplorer.cpp>
#include <Chaos/Debug/ImGuiDebugInfo.cpp>
#include <Chaos/Debug/Timer.cpp>

//===NODE=====================================
#include <Chaos/Nodes/Node.cpp>
#include <Chaos/Nodes/Camera.cpp>
#include <Chaos/Nodes/Sprite.cpp>

//===PLATFORM==================================
#include <Platform/Windows/WindowsWindow.cpp>
#include <Platform/Windows/WindowsInput.cpp>
#include <Platform/Vulkan/VulkanTexture.cpp>
#include <Platform/Vulkan/VulkanSubTexture.cpp>
#include <Platform/Vulkan/VulkanRenderer.cpp>

//===INPUT=====================================
#include <Chaos/Input/InputManager.cpp>

//===RENDERER===================================
#include <Chaos/Renderer/RendererAPI.cpp>
#include <Chaos/Renderer/Renderer.cpp>
#include <Chaos/Renderer/Texture.cpp>
#include <Chaos/Renderer/SubTexture.cpp>

//===SERIALISATION=============================
#include <Chaos/Serialisation/TextFileReader.cpp>