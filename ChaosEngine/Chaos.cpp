
//Unity build file for easy compilation outside of visual studio
#include <chaospch.h>
#include <Chaos.h>

//===CORE=======================================
#include <Chaos/Core/Application.cpp>
#include <Chaos/Core/LayerStack.cpp>
#include <Chaos/Core/Log.cpp>
#include <Chaos/Core/Scene.cpp>
#include <Chaos/Core/SceneManager.cpp>
#include <Chaos/Core/Time.cpp>

//===DATA TYPES================================
#include <Chaos/DataTypes/Ray2D.cpp>

//===DEBUG======================================
#include <Chaos/Debug/Console.cpp>
#include <Chaos/Debug/ImGuiBuild.cpp>
#include <Chaos/Debug/ImGuiLayer.cpp>
#include <Chaos/Debug/ImGuiConsole.cpp>
#include <Chaos/Debug/Timer.cpp>

//===ENTITY=====================================
#include <Chaos/Entity/ECSManager.cpp>

//===PLATFORM==================================
#include <Platform/Windows/WindowsWindow.cpp>
#include <Platform/Windows/WindowsInput.cpp>
#include <Platform/Vulkan/VulkanTexture.cpp>
#include <Platform/Vulkan/VulkanSubTexture.cpp>
#include <Platform/Vulkan/VulkanRenderer.cpp>

//===COMPONENTS=================================
#include <Chaos/Entity/Components/Camera.cpp>
#include <Chaos/Entity/Components/Transform.cpp>
#include <Chaos/Entity/Components/Render.cpp>
#include <Chaos/Entity/Components/SubRender.cpp>
#include <Chaos/Entity/Components/CellularAutomata.cpp>
#include <Chaos/Entity/Components/BoxCollider2D.cpp>
#include <Chaos/Entity/Components/UIImage.cpp>
#include <Chaos/Entity/Components/Animator.cpp>

//===RENDERER===================================
#include <Chaos/Renderer/RendererAPI.cpp>
#include <Chaos/Renderer/Renderer.cpp>
#include <Chaos/Renderer/Texture.cpp>
#include <Chaos/Renderer/SubTexture.cpp>
