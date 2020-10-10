/* date = October 8th 2020 5:04 pm */

#ifndef _IM_GUI_EDITOR_H
#define _IM_GUI_EDITOR_H
#include <Chaos/Debug/ImGuiLayer.h>
#include <Chaos/Entity/Components/EditorCameraController.h>
#include <Chaos/DataTypes/Vec2.h>
#include <Chaos/Renderer/Texture.h>

namespace Chaos
{
	class ImGuiEditor : public ImGuiLayer
	{
		public:
		ImGuiEditor();
		
		virtual void OnImGuiUpdate() override;
		
		void ShowEditor();
		void ShowDetails();
		void UpdateSelectedEntity();
		
		private:
		bool IsSelected(Entity* entity);
		
		private:
		bool m_showEditor = false;
		bool m_clicked = false;
		
		char* m_filePathInput = "";
		std::string m_filePath = "";
		
		const Vec2 DETAILS_WINDOW_SIZE = Vec2(300,400);
		const Vec2 DETAILS_WINDOW_PADDING = Vec2(50,80);
		
		Vec2 m_draggingEntPositionOffset = Vec2::Zero();
		std::vector<Entity*> m_selectedEntities;
		EditorCameraController* m_cameraController;
		ImVec2 m_editorWindowPos;
		ImVec2 m_editorWindowSize;
		
		Ref<Texture> m_selectedEntTexture = Texture::Create("");
		ImTextureID m_selectedEntTextureID;
	};
}

#endif //_IM_GUI_EDITOR_H

