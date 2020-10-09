/* date = October 8th 2020 5:04 pm */

#ifndef _IM_GUI_EDITOR_H
#define _IM_GUI_EDITOR_H
#include <Chaos/Debug/ImGuiLayer.h>
#include <Chaos/Entity/Components/EditorCameraController.h>
#include <Chaos/DataTypes/Vec2.h>
namespace Chaos
{
	class ImGuiEditor : public ImGuiLayer
	{
		public:
		ImGuiEditor();
		
		virtual void OnImGuiUpdate() override;
		static void OpenEditor() ; 
		
		private:
		bool IsSelected(Entity* entity);
		
		private:
		static bool m_showEditor;
		
		bool m_clicked = false;
		
		char* m_filePathInput = "";
		std::string m_filePath = "";
		
		Vec2 m_draggingEntPositionOffset = Vec2::Zero();
		std::vector<Entity*> m_selectedEntities;
		EditorCameraController* m_cameraController;
		
	};
}

#endif //_IM_GUI_EDITOR_H

