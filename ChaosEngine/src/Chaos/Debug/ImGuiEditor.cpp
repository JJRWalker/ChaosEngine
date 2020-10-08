#include <chaospch.h>
#include <Chaos/Debug/ImGuiEditor.h>
#include <Chaos/Debug/Console.h>
#include <Chaos/Entity/Components/BoxCollider2D.h>

namespace Chaos
{
	bool ImGuiEditor::m_showEditor = false;
	ImGuiEditor::ImGuiEditor()
	{
		Console::AddCommand("editor", &OpenEditor);
		Application::Get().GetMainCamera()->GetEntity()->AddComponent<EditorCameraController>();
		m_cameraController = Application::Get().GetMainCamera()->GetEntity()->GetComponent<EditorCameraController>();
		m_cameraController->SetActive(false);
	}
	
	void ImGuiEditor::OnImGuiUpdate() 
	{
		if (m_showEditor)
		{
			if (!m_cameraController->GetActive())
			{
				m_cameraController->SetActive(true);
			}
			ImGuiWindowFlags window_flags =  ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;
			
			//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.1f));
			ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
			ImGui::Begin("Level", NULL, window_flags);
			
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();
			
			//ImGui::PopStyleColor();
			if(ImGui::BeginMenuBar())
			{
				if(ImGui::BeginMenu("add.."))
				{
					if(ImGui::MenuItem("ent"))
					{
						LOGINFO("ENT!");
					}
					ImGui::EndMenu();
				}
				if(ImGui::Button("save.."))
				{
					LOGINFO("Save...");
				}
				if(ImGui::Button("load..."))
				{
					LOGINFO("Load...");
				}
				if(ImGui::Button("close"))
				{
					m_showEditor = false;
					m_cameraController->SetActive(false);
				}
				
				ImGui::EndMenuBar();
				
				ImGui::BeginChild("ScrollingRegion", ImVec2(0, -30), false, ImGuiWindowFlags_HorizontalScrollbar);
				
				static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
				
				for(int i = 0; i < SceneManager::GetScene()->GetEntities().size(); ++i)
				{
					bool nodeClicked = false;
					Entity* entity = SceneManager::GetScene()->GetEntities()[i];
					ImGuiTreeNodeFlags nodeFlags = baseFlags;
					if(IsSelected(entity))
					{
						nodeFlags |= ImGuiTreeNodeFlags_Selected;
					}
					
					nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "%s", entity->GetName());
					if (ImGui::IsItemClicked())
						nodeClicked = true;
					
					if (nodeClicked)
					{
						if (ImGui::GetIO().KeyCtrl)
						{
							m_selectedEntities.push_back(entity);         // CTRL+click to toggle
						}
						else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
						{
							m_selectedEntities.clear();
							m_selectedEntities.push_back(entity);	// Click to single-select
						}
					}
				}
				
				if(Input::IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					Vec2 mouseWorldPoint =  Application::Get().GetMainCamera()->ScreenToWorld(Input::GetMousePosition());
					
					
					
					for (Entity* ent : SceneManager::GetScene()->GetEntities())
					{
						if (ent->HasComponent<BoxCollider2D>())
						{
							if(ent->GetComponent<BoxCollider2D>()->PointCollision(mouseWorldPoint))
							{
								//only add if selected entities doesn't already contain it
								if (std::find(m_selectedEntities.begin(), m_selectedEntities.end(), ent) == m_selectedEntities.end())
								{
									m_selectedEntities.push_back(ent);
								}
							}
						}
					}
				}
				
				if (m_selectedEntities.size() > 0)
				{
					Entity* entity = m_selectedEntities[m_selectedEntities.size() - 1];
					
					ImGui::SetNextWindowSize(ImVec2(300, 400));
					ImGui::SetNextWindowPos(ImVec2(windowPos.x + (windowSize.x), windowPos.y));
					ImGui::Begin("Details");
					//put detail information here...
					
					float* pos[2] = { &entity->GetTransform()->Position().X, &entity->GetTransform()->Position().Y };
					float* scale[2] = { &entity->GetTransform()->Scale().X, &entity->GetTransform()->Scale().Y };
					
					ImGui::DragFloat2("Position", *pos, 0.01f);
					
					ImGui::DragFloat2("Scale", *scale, 0.01f);
					
					ImGui::DragFloat("Rotation", &entity->GetScale().X, 1.0f, -180, 180);
					
					ImGui::Separator();
					
					ImGui::End();
				}
				
				ImGui::EndChild();
				
			}
			ImGui::End();
			
		}
	}
	
	bool ImGuiEditor::IsSelected(Entity* entity)
	{
		for (auto e : m_selectedEntities)
		{
			if (e->GetEntityID() == entity->GetEntityID())
				return true;
		}
		return false;
	}
	
	void ImGuiEditor::OpenEditor()
	{
		m_showEditor = true;
	}
	
}