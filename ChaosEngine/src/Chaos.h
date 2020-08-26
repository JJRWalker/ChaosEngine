#pragma once

//FOR USE BY CHAOS APPS

//===CORE===============================================
#include "Chaos/Core/Application.h"
#include "Chaos/Core/Core.h"
#include "Chaos/Core/Layer.h"
#include "Chaos/Core/LayerStack.h"
#include "Chaos/Core/Log.h"
#include "Chaos/Core/Scene.h"
#include "Chaos/Core/Time.h"
#include "Chaos/Core/Window.h"

//===INPUT==============================================
#include "Chaos/Input/Input.h"

//===DATATYPES==========================================
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec3.h"
#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/DataTypes/Ray2D.h"

//===DEBUG==============================================
#include "Chaos/Debug/ImGuiLayer.h"
#include "Chaos/Debug/Timer.h"

//===ENTITY=============================================
#include "Chaos/Entity/Entity.h"
#include "Chaos/Entity/ECSManager.h"

//===COMPONENTS========================================
#include "Chaos/Entity/Components/Camera.h"
#include "Chaos/Entity/Components/Transform.h"
#include "Chaos/Entity/Components/Render.h"
#include "Chaos/Entity/Components/UIImage.h"
#include "Chaos/Entity/Components/BoxCollider2D.h"

//===RENDERING==========================================
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Renderer/SubTexture.h"

//===EVENTS=============================================
#include "Chaos/Events/ApplicationEvent.h"
#include "Chaos/Events/KeyEvent.h"
#include "Chaos/Events/MouseEvent.h"

//===PLATFORM===========================================
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanTexture.h"
#include "Platform/Vulkan/VulkanSubTexture.h"

#include "Platform/Windows/WindowsWindow.h"