#ifndef _CHAOS_H
#define _CHOAS_H

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
#include "Chaos/Core/SceneManager.h"

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

//===EVENTS=============================================
#include "Chaos/Events/ApplicationEvent.h"
#include "Chaos/Events/KeyEvent.h"
#include "Chaos/Events/MouseEvent.h"
#endif