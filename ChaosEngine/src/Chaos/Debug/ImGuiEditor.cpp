#include <chaospch.h>
#include <Chaos/Debug/ImGuiEditor.h>
#include <Chaos/Debug/Console.h>
#include <Chaos/Nodes/Camera.h>
#include <Chaos/Nodes/Colliders.h>
#include <Chaos/Nodes/Sprite.h>
#include <Chaos/Debug/ImGuiFileExplorer.h>
#include <Chaos/Core/Application.h>
#include <Chaos/Core/Time.h>
#include <Chaos/Input/Input.h>
#include <Chaos/Maths/Collisions.h>
#include <Chaos/Renderer/Renderer.h>
#include <Chaos/Nodes/MeshRenderer.h>


namespace Chaos
{
	ImGuiEditor::ImGuiEditor()
	{
		Console::AddCommand("ed", [&](){ 
								if (m_showEditor)
								{
									Time::SetTimeScale(m_timeScaleBeforePause);
									m_showEditor = false;
								}
								else
								{
									m_timeScaleBeforePause = Time::GetTimeScale();
									Time::SetTimeScale(0.0f);
									m_showEditor = true;
								}
							});
		
		Console::AddCommand("demogui", [&](){ m_showDemo ? m_showDemo = false : m_showDemo = true;});
		
		Console::AddCommand("play", [&](){ 
								Application::Get().Play();
								m_selectedEntities.clear();
							});
		Console::AddCommand("end", [&](){ 
								Application::Get().EndPlay();
								m_selectedEntities.clear();
							});
		
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
			
			Input::ButtonsEnabled = false;
			
			MoveCamera(Time::GetUnscaledDeltaTime());
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
		Level* level = Level::Get();
		
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
				if(ImGui::MenuItem("Node"))
				{
					Node* node = new Node();
				}
				if(ImGui::MenuItem("Sprite"))
				{
					Node* node = new Sprite();
				}
				if(ImGui::MenuItem("Sub-Sprite"))
				{
					Node* node = new SubSprite();
				}
				if(ImGui::MenuItem("Box-Colldier-2D"))
				{
					Node* node = new BoxCollider2D();
				}
				if(ImGui::MenuItem("Camera"))
				{
					Node* node = new Camera();
				}
				if(ImGui::MenuItem("Mesh-Renderer"))
				{
					Node* node = new MeshRenderer();
				}
				ImGui::EndMenu();
			}
			if(ImGui::Button("save.."))
			{
				LOGINFO("Save...");
				Level::Save("./test-level.lvl");  //TODO: Pass in path through file dialog
			}
			if(ImGui::Button("load..."))
			{
				LOGINFO("Load...");
				Level::Load("./test-level.lvl");  //TODO: Pass in path through file dialog
				m_selectedEntities.clear();
			}
			if(ImGui::Button("close"))
			{
				m_showEditor = false;
				Input::ButtonsEnabled = true;
				Time::SetTimeScale(m_timeScaleBeforePause);
			}
			
			ImGui::EndMenuBar();
			
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -30), false, ImGuiWindowFlags_HorizontalScrollbar);
			
			static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			
			int nodeSelected = -1;
			int childSelected = 0;
			
			for(int i = 0; i < Level::Get()->NodeCount; ++i)
			{
				
				bool nodeClicked = false;
				Node* node = Level::Get()->Nodes[i];
				ImGuiTreeNodeFlags nodeFlags = baseFlags;
				if(IsSelected(node))
				{
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				}
				
				if (node->Children.Size() < 1)
				{
					nodeFlags |= ImGuiTreeNodeFlags_Leaf;
				}
				
				if (ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "%s", node->Name.c_str()))
				{
					ImGui::TreePop();
				}
				
				
				if (ImGui::IsItemClicked())
					nodeSelected = i;
				
				if (nodeSelected != -1)
				{
					Node* newSelected = level->Nodes[nodeSelected];
					if (ImGui::GetIO().KeyCtrl)
					{
						for (int child = 0; child < newSelected->Children.Size(); ++child)
						{
							newSelected->Children[child]->DebugEnabled = true;
						}
						m_selectedEntities.push_back(newSelected);         // CTRL+click to toggle
					}
					else if (ImGui::GetIO().KeyShift)
					{
						int selectionStartIndex = m_selectedEntities[m_selectedEntities.size() - 1]->ID;
						
						int selectionEndIndex = nodeSelected;
						
						if (nodeSelected < selectionStartIndex)
						{
							int temp = selectionEndIndex;
							selectionEndIndex = selectionStartIndex;
							selectionStartIndex = temp;
						}
						
						for (int node = selectionStartIndex; node <= selectionEndIndex; ++node)
						{
							m_selectedEntities.push_back(level->Nodes[node]);
							
							for (int child = 0; child < level->Nodes[node]->Children.Size(); ++child)
							{
								level->Nodes[node]->Children[child]->DebugEnabled = true;
							}
						}
						
					}
					else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
					{
						for (Node* selected : m_selectedEntities)
						{
							for (int child = 0; child < selected->Children.Size(); ++child)
							{
								selected->Children[child]->DebugEnabled = false;
							}
						}
						m_selectedEntities.clear();
						
						newSelected->DebugEnabled = true;
						
						for (int child = 0; child < newSelected->Children.Size(); ++child)
						{
							newSelected->Children[child]->DebugEnabled = true;
						}
						
						m_selectedEntities.push_back(newSelected);	// Click to single-select
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
		
		Level* level = Level::Get();
		
		ImGui::SetNextWindowSize(ImVec2(DETAILS_WINDOW_SIZE.X + DETAILS_WINDOW_PADDING.X, DETAILS_WINDOW_SIZE.Y + DETAILS_WINDOW_PADDING.Y));
		ImGui::SetNextWindowPos(ImVec2(m_editorWindowPos.x + (m_editorWindowSize.x), m_editorWindowPos.y));
		ImGui::Begin("Details");
		
		//generic node data...
		float pos[3] = { node->GetPosition().X, node->GetPosition().Y, node->GetDepth() };
		float scale[2] = { node->GetScale().X, node->GetScale().Y };
		float rot = node->GetRotation();
		
		bool enabled = node->IsEnabled();
		
		
		if (ImGui::Checkbox("", &enabled))
		{
			node->SetEnabled(enabled);
			if (node->Children.Size())
			{
				for (int child = 0; child < node->Children.Size(); ++child)
				{
					level->Nodes[node->ID]->Children[child]->SetEnabled(enabled);
				}
			}
		}
		ImGui::SameLine();
		
		ImGui::Text("%s", node->Name.c_str()); 
		
		ImGui::SameLine();
		
		ImGui::Text("\t ID: %d", node->ID); 
		
		ImGui::Text("%s", node->GetType());
		
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
		if (ImGui::Button("Delete") || Input::IsKeyPressed(KEY_DELETE))
		{
			for (Node* toDelete : m_selectedEntities)
				toDelete->Destroy();
			
			m_selectedEntities.clear();
		}
		
		ImGui::Separator();
		
		if(node)
			node->OnShowEditorDetails(m_selectedEntTexture, m_selectedEntTextureID);
		
		ImGui::End();
	}
	
	
	
	void ImGuiEditor::UpdateSelectedEntity()
	{
		if (ImGui::IsAnyWindowHovered() || !Level::Get()->MainCamera)
			return;
		
		Vec2 mouseWorldPoint =  Level::Get()->MainCamera->ScreenToWorld(Input::GetMousePosition());
		
		if(Input::IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !m_clicked)
		{
			//Not sure about how nested this gets..
			//when editor is open if we click find the box collider where the world mouse position that click intersects
			
			for (int i = 0; i < Level::Get()->NodeCount; ++i)
			{
				Node* node = Level::Get()->Nodes[i];
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
				Node* node = Level::Get()->Nodes[i];
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
	
	
	void ImGuiEditor::MoveCamera(float delta)
	{
		Vec2 dir = Vec2::Zero();
		if(Input::IsKeyPressed(KEY_W))
		{
			dir.Y = 1;
		}
		else if (Input::IsKeyPressed(KEY_S))
		{
			dir.Y = -1;
		}
		else
		{
			dir.Y = 0;
		}
		
		if(Input::IsKeyPressed(KEY_A))
		{
			dir.X = -1;
		}
		else if (Input::IsKeyPressed(KEY_D))
		{
			dir.X = 1;
		}
		else
		{
			dir.X = 0;
		}
		
		Vec2 position = Level::Get()->MainCamera->GetPosition() + (dir * (m_cameraSpeed * delta));
		
		Level::Get()->MainCamera->SetPosition(position);
	}
	
	
	bool ImGuiEditor::IsSelected(Node* entity)
	{
		for (auto e : m_selectedEntities)
		{
			if (e->ID == entity->ID)
				return true;
		}
		return false;
	}
	
}