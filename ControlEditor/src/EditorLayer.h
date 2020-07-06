#pragma once
#include <Chaos.h>
#include <ImGUI/imgui.h>

#include "Camera/CameraController.h"
#include <Chaos/Entity/Entity.h>

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
		float m_time = 0.0f;

		ImTextureID m_veiwportImageId;
		Vec2 m_ViewportSize;
		Entity entity;
		CameraController m_cameraController;
	};
}