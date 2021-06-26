#include <chaospch.h>
#include <Chaos/Debug/ImGuiEditor.h>
#include <Chaos/Debug/Console.h>
#include <Chaos/Nodes/Camera.h>
#include <Chaos/Nodes/Colliders.h>
#include <Chaos/Nodes/Sprite.h>
#include <Chaos/Nodes/EditorCameraController.h>
#include <Chaos/Debug/ImGuiFileExplorer.h>
#include <Chaos/Core/Application.h>
#include <Chaos/Input/Input.h>
#include <Chaos/Maths/Collisions.h>
#include <Chaos/Renderer/Renderer.h>

namespace Chaos
{
	ImGuiEditor::ImGuiEditor()
	{
		Console::AddCommand("ed", [&](){ m_showEditor ? m_showEditor = false : m_showEditor = true;});
		Console::AddCommand("demogui", [&](){ m_showDemo ? m_showDemo = false : m_showDemo = true;});
		m_cameraController = Level::Get()->MainCamera()->AddChild<EditorCameraController>();
		m_cameraController->SetEnabled(false);
		
		m_selectedEntTextureID = Application::Get().GetRenderer().GetImguiEditorPanelTextureID();
	}
	
	void ImGuiEditor::OnImGuiUpdate() 
	{
		if (m_showDemo)
		{
			ImGui::ShowDemoWindow();
		}
		if (m_showEditor)
		{
			//show the main body of the editor (level heirarchy)
			ShowEditor();
			//update inputs and then determine if an entity was clicked
			UpdateSelectedEntity();
			//if an entitiy is selected show the details pannel
			if (m_selectedEntities.size() > 0)
			{
				ShowDetails();
			}
		}
	}
	
	void ImGuiEditor::ShowEditor()
	{
		if (!m_cameraController->IsEnabled())
		{
			//m_cameraController->SetEnabled(true);
		}
		ImGuiWindowFlags window_flags =  ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;
		
		//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.1f));
		ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
		ImGui::Begin("Level", NULL, window_flags);
		
		m_editorWindowPos = ImGui::GetWindowPos();
		m_editorWindowSize = ImGui::GetWindowSize();
		
		//ImGui::PopStyleColor();
		if(ImGui::BeginMenuBar())
		{
			if(ImGui::BeginMenu("add.."))
			{
				if(ImGui::MenuItem("Sprite"))
				{
					Node* node = new Sprite();
				}
				if(ImGui::MenuItem("Sub-Sprite"))
				{
					Node* node = new SubSprite();
				}
				ImGui::EndMenu();
			}
			if(ImGui::Button("save.."))
			{
				LOGINFO("Save...");
				Level::Get()->Save("./test-level.lvl");  //TODO: Pass in path through file dialog
			}
			if(ImGui::Button("load..."))
			{
				LOGINFO("Load...");
				Level::Get()->Load("./test-level.lvl");  //TODO: Pass in path through file dialog
			}
			if(ImGui::Button("close"))
			{
				m_showEditor = false;
				m_cameraController = false;
			}
			
			ImGui::EndMenuBar();
			
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -30), false, ImGuiWindowFlags_HorizontalScrollbar);
			
			static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			
			int nodeSelected = -1;
			int childSelected = 0;
			
			for(int i = 0; i < Level::Get()->NodeCount; ++i)
			{
				
				bool nodeClicked = false;
				Node* node = Level::Get()->Nodes[i][0];
				ImGuiTreeNodeFlags nodeFlags = baseFlags;
				if(IsSelected(node))
				{
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				}
				
				bool openTree = ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "%s", node->Name);
				
				if (ImGui::IsItemClicked())
					nodeSelected = i;
				
				if (openTree)
				{
					for (int j = 1; j <= Level::Get()->Nodes[i][0]->ChildCount; ++j)
					{
						ImGuiTreeNodeFlags childFlags = baseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
						
						Node* child = Level::Get()->Nodes[i][j];
						
						if(IsSelected(child))
						{
							childFlags |= ImGuiTreeNodeFlags_Selected;
						}
						
						ImGui::TreeNodeEx((void*)(intptr_t)j, childFlags, "%s", Level::Get()->Nodes[i][j]->Name);
						
						
						
						if (ImGui::IsItemClicked())
						{
							nodeSelected = i;
							childSelected = j;
						}
						
					}
					
					ImGui::TreePop();
				}
				
				
				
				if (nodeSelected != -1)
				{
					if (ImGui::GetIO().KeyCtrl)
					{
						m_selectedEntities.push_back(Level::Get()->Nodes[nodeSelected][childSelected]);         // CTRL+click to toggle
					}
					else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
					{
						for (Node* selected : m_selectedEntities)
						{
							selected->DebugEnabled = false;
						}
						m_selectedEntities.clear();
						m_selectedEntities.push_back(Level::Get()->Nodes[nodeSelected][childSelected]);	// Click to single-select
					}
					
				}
			}
			ImGui::EndChild();
			ImGui::End();
		}
	}
	
	void ImGuiEditor::ShowDetails()
	{
		Node* node = m_selectedEntities[m_selectedEntities.size() - 1];
		
		node->DebugEnabled = true;
		
		ImGui::SetNextWindowSize(ImVec2(DETAILS_WINDOW_SIZE.X + DETAILS_WINDOW_PADDING.X, DETAILS_WINDOW_SIZE.Y + DETAILS_WINDOW_PADDING.Y));
		ImGui::SetNextWindowPos(ImVec2(m_editorWindowPos.x + (m_editorWindowSize.x), m_editorWindowPos.y));
		ImGui::Begin("Details");
		//generic node data...
		
		float pos[3] = { node->GetPosition().X, node->GetPosition().Y, node->GetDepth() };
		float scale[2] = { node->GetScale().X, node->GetScale().Y };
		float rot = node->GetRotation();
		
		if (ImGui::DragFloat3("Position", pos, 0.01f))
		{
			node->SetPosition(Vec3(pos[0], pos[1], pos[2]));
		}
		if (ImGui::DragFloat2("Scale", scale, 0.01f))
		{
			node->SetScale(Vec2(scale[0], scale[1]));
		}
		if (ImGui::DragFloat("Rotation", &rot, 0.01f))
		{
			node->SetRotation(rot);
		}
		
		ImGui::Separator();
		
		//Render component..
		//currently hard coded inputs in future should be automatic depending on the variable type
		
		node->OnShowEditorDetails(m_selectedEntTexture, m_selectedEntTextureID);
		
		ImGui::End();
	}
	
	
	
	void ImGuiEditor::UpdateSelectedEntity()
	{
		if (ImGui::IsAnyWindowHovered())
			return;
		
		Vec2 mouseWorldPoint =  Level::Get()->MainCamera()->ScreenToWorld(Input::GetMousePosition());
		
		if(Input::IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !m_clicked)
		{
			//Not sure about how nested this gets..
			//when editor is open if we click find the box collider where the world mouse position that click intersects
			
			for (int i = 0; i < Level::Get()->NodeCount; ++i)
			{
				Node* node = Level::Get()->Nodes[i][0];
				if (node->HasChild<BoxCollider2D>())
				{
					BoxCollider2D* collider = node->GetChild<BoxCollider2D>();
					if (Collisions::PointInRectangle(mouseWorldPoint, collider->GetPosition(), collider->Bounds))
					{
						if (!m_clicked)
						{
							m_draggingEntPositionOffset = Vec2(mouseWorldPoint.X - node->GetPosition().X, mouseWorldPoint.Y - node->GetPosition().Y);
							m_clicked = true;
						}
						
						if (std::find(m_selectedEntities.begin(), m_selectedEntities.end(), node) == m_selectedEntities.end())
						{
							m_selectedEntities.clear();
							m_selectedEntities.push_back(node);
							
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
		
		if(Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			//Not sure about how nested this gets..
			//when editor is open if we click find the box collider where the world mouse position that click intersects
			
			for (int i = 0; i < Level::Get()->NodeCount; ++i)
			{
				Node* node = Level::Get()->Nodes[i][0];
				if (node->HasChild<BoxCollider2D>())
				{
					BoxCollider2D* collider = node->GetChild<BoxCollider2D>();
					if (Collisions::PointInRectangle(mouseWorldPoint, collider->GetPosition(), collider->Bounds))
					{
						
						if (std::find(m_selectedEntities.begin(), m_selectedEntities.end(), node) == m_selectedEntities.end())
						{
							m_selectedEntities.clear();
							m_selectedEntities.push_back(node);
							
						}
						
						//clear entities and select just this one
						//TODO: in future support group selecting entites
						
					}
				}
			}
			
		}
		
		if (m_clicked)
		{
			Vec2 position = Vec2(mouseWorldPoint.X - m_draggingEntPositionOffset.X, mouseWorldPoint.Y - m_draggingEntPositionOffset.Y);
			m_selectedEntities[m_selectedEntities.size() - 1]->SetPosition(position);
			
		}
	}
	
	bool ImGuiEditor::IsSelected(Node* entity)
	{
		for (auto e : m_selectedEntities)
		{
			if (e->ID == entity->ID && e->SubID == entity->SubID)
				return true;
		}
		return false;
	}
	
}