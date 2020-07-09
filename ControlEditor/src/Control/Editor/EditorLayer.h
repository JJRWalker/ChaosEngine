#pragma once
#include <Chaos.h>
#include <ImGUI/imgui.h>

#include "Control/Camera/CameraController.h"
#include <Chaos/Entity/Entity.h>
#include <Control/Editor/EditorScene.h>

namespace Chaos
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override;
		virtual void OnEvent(Event& event) override;
		virtual void OnImGuiUpdate() override;
	private:
		void CreateDocker();
		void CreateSceneHierarchy();
		void CreateViewPort();
		void CreateSettings();
		void CreateInspector();
		void CreateProfiler();

		bool IsSelected(Entity* entity);
	private:
		float m_time = 0.0f;
		bool m_viewportHovered = false;
		bool m_viewportFocused = false;
		bool m_profilerOpen = true;

		ImTextureID m_veiwportImageId;
		Vec2 m_ViewportSize;
		EditorScene m_scene;
		CameraController m_cameraController;

		std::vector<Entity*> m_selectedEntities;
	};
}