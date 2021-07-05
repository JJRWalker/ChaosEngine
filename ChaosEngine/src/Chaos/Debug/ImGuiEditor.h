/* date = October 8th 2020 5:04 pm */

#ifndef _IM_GUI_EDITOR_H
#define _IM_GUI_EDITOR_H
#include <Chaos/Debug/ImGuiLayer.h>
#include <Chaos/DataTypes/Vec2.h>
#include <Chaos/Renderer/Texture.h>

namespace Chaos
{
	const Vec2 DETAILS_WINDOW_SIZE = Vec2(300,400);
	const Vec2 DETAILS_WINDOW_PADDING = Vec2(50,80);
	
	class Node;
	class EditorCameraController;
	
	class ImGuiEditor : public ImGuiLayer
	{
		public:
		ImGuiEditor();
		
		virtual void OnImGuiUpdate() override;
		
		void ShowEditor();
		void ShowDetails();
		void ShowLevelSettings();
		void UpdateSelectedEntity();
		
		void MoveCamera(float delta);
		void DrawSelectedWidget();
		
		private:
		bool CreateTreeNode(Node* node, ImGuiTreeNodeFlags nodeFlags);
		void OnNodeSelected(Node* node);
		bool IsSelected(Node* node);
		
		private:
		bool m_showDemo = false;
		
		// if editor is defined, start the editor in the open state
#ifdef CHAOS_EDITOR
		bool m_showEditor = true;
#else
		bool m_showEditor = false;
#endif
		bool m_showLevelSettings = false;
		bool m_clicked = false;
		bool m_dragging = false;
		
		float m_timeScaleBeforePause = 1.0f;
		float m_cameraSpeed = 10.0f;
		
		char* m_filePathInput = "";
		std::string m_filePath = "";
		
		Vec2 m_draggingEntPositionOffset = Vec2::Zero();
		std::vector<Node*> m_selectedEntities;
		EditorCameraController* m_cameraController;
		ImVec2 m_editorWindowPos;
		ImVec2 m_editorWindowSize;
		
		Texture* m_selectedEntTexture = Texture::CreateBlank("Debug_SelectedEnt");
		ImTextureID m_selectedEntTextureID;
	};
}

#endif //_IM_GUI_EDITOR_H

