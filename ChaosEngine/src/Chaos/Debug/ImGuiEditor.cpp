#include <chaospch.h>
#include <Chaos/Debug/ImGuiEditor.h>
#include <Chaos/Debug/Console.h>
#include <Chaos/Entity/Components/Camera.h>
#include <Chaos/Entity/Components/BoxCollider2D.h>
#include <Chaos/Entity/Components/Render.h>
#include <Chaos/Entity/Components/SubRender.h>
#include <Chaos/Debug/ImGuiFileExplorer.h>
#include <Chaos/Core/Application.h>
#include <Chaos/Core/SceneManager.h>
#include <Chaos/Input/Input.h>

namespace Chaos
{
	bool ImGuiEditor::m_showEditor = false;
	ImGuiEditor::ImGuiEditor()
	{
		Console::AddCommand("ed", &OpenEditor);
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
						Entity* entity = new Entity();
						entity->AddComponent<Render>();
						entity->AddComponent<BoxCollider2D>();
						entity->GetComponent<BoxCollider2D>()->SetTrigger(true);
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
				
				
				Vec2 mouseWorldPoint =  Application::Get().GetMainCamera()->ScreenToWorld(Input::GetMousePosition());
				
				if(Input::IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					//Not sure about how nested this gets..
					//when editor is open if we click find the box collider where the world mouse position that click intersects
					
					
					for (Entity* ent : SceneManager::GetScene()->GetEntities())
					{
						if (ent->HasComponent<BoxCollider2D>())
						{
							if(ent->GetComponent<BoxCollider2D>()->PointCollision(mouseWorldPoint))
							{
								if(!m_clicked)
								{
									m_draggingEntPositionOffset = Vec2(mouseWorldPoint.X - ent->GetPosition().X, mouseWorldPoint.Y - ent->GetPosition().Y);
									m_clicked = true;
								}
								
								if(std::find(m_selectedEntities.begin(), m_selectedEntities.end(), ent) == m_selectedEntities.end())
								{
									m_selectedEntities.clear();
									m_selectedEntities.push_back(ent);
									
								}
								
								//clear entities and select just this one
								//TODO: in future support group selecting entites
								
							}
						}
					}
				}
				else if (Input::IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
				{
					m_draggingEntPositionOffset = Vec2::Zero();
					m_clicked = false;
				}
				
				if (m_clicked)
				{
					m_selectedEntities[m_selectedEntities.size() - 1]->GetPosition() = Vec3 (mouseWorldPoint.X - m_draggingEntPositionOffset.X, mouseWorldPoint.Y - m_draggingEntPositionOffset.Y, m_selectedEntities[m_selectedEntities.size() - 1]->GetPosition().Z);
					
				}
				
				if (m_selectedEntities.size() > 0)
				{
					Entity* entity = m_selectedEntities[m_selectedEntities.size() - 1];
					
					entity->Debug();
					
					ImGui::SetNextWindowSize(ImVec2(300, 400));
					ImGui::SetNextWindowPos(ImVec2(windowPos.x + (windowSize.x), windowPos.y));
					ImGui::Begin("Details");
					//put detail information here...
					
					float* pos[2] = { &entity->GetTransform()->Position().X, &entity->GetTransform()->Position().Y };
					float* scale[2] = { &entity->GetTransform()->Scale().X, &entity->GetTransform()->Scale().Y };
					
					ImGui::DragFloat2("Position", *pos, 0.01f);
					
					ImGui::DragFloat2("Scale", *scale, 0.01f);
					
					ImGui::DragFloat("Rotation", &entity->GetRotation().X, 1.0f, -180, 180);
					
					ImGui::Separator();
					
					//Render component..
					//currently hard coded inputs in future should be automatic depending on the variable type
					
					if(entity->HasComponent<Render>())
					{
						
						if (ImGui::Button("Change texture"))
						{
							std::string path = entity->GetComponent<Render>()->GetTexture()->GetFilePath();
							if(path != "")
								m_filePath = path;
							Application::Get().PushOverlay(new ImGuiFileExplorer(m_filePath, [&](){m_selectedEntities[m_selectedEntities.size() - 1]->GetComponent<Render>()->GetTexture()->Load(m_filePath);} ));
						}
						
						ImGui::Text(m_filePath.c_str());
					}
					if (entity->HasComponent<SubRender>())
					{
						SubRender* subrender = entity->GetComponent<SubRender>();
						float* coord[2] = { &subrender->GetSubTexture()->GetCellCoords().X, &subrender->GetSubTexture()->GetCellCoords().Y };
						
						float* size[2] = { &subrender->GetSubTexture()->GetCellSize().X, &subrender->GetSubTexture()->GetCellSize().Y };
						
						ImGui::DragFloat2("Sprite Coordinate", *coord, 1);
						
						ImGui::DragFloat2("Cell Size", *size, 1);
						
						if (ImGui::Button("Change texture"))
						{
							std::string path = entity->GetComponent<SubRender>()->GetSubTexture()->GetFilePath();
							if(path != "")
								m_filePath = path;
							Application::Get().PushOverlay(new ImGuiFileExplorer(m_filePath, [&](){m_selectedEntities[m_selectedEntities.size() - 1]->GetComponent<SubRender>()->SetSubTexture(SubTexture::Create(m_filePath, Vec2(0,0), Vec2(32,32)));} ));
						}
						ImGui::SameLine();
						if (ImGui::Button("Set Coords"))
						{
							subrender->GetSubTexture()->SetTexCoords(subrender->GetSubTexture()->GetCellCoords(), subrender->GetSubTexture()->GetCellSize());
						}
						
					}
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