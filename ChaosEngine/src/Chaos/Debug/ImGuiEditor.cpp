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
#include <Platform/Vulkan/VulkanRenderer.h>
#include <Platform/Vulkan/VulkanTexture.h>
#include <ImGUI/examples/imgui_impl_vulkan.h>
#include <ImGUI/examples/imgui_impl_glfw.h>

namespace Chaos
{
	ImGuiEditor::ImGuiEditor()
	{
		Console::AddCommand("ed", [&](){ m_showEditor ? m_showEditor = false : m_showEditor = true; });
		m_cameraController = Level::Get()->MainCamera()->AddChild<EditorCameraController>();
		m_cameraController->Enabled = false;
		
		VulkanRenderer& renderer = (VulkanRenderer&)Application::Get().GetRenderer();
		VulkanTexture* vulkanTex = (VulkanTexture*)m_selectedEntTexture.get();
		
		m_selectedEntTextureID = (ImTextureID)ImGui_ImplVulkan_AddTexture(renderer.GetTexSampler(), vulkanTex->GetImageView(), VK_IMAGE_LAYOUT_UNDEFINED);
		
	}
	
	void ImGuiEditor::OnImGuiUpdate() 
	{
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
		if (!m_cameraController->Enabled)
		{
			m_cameraController->Enabled = true;
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
				if(ImGui::MenuItem("ent"))
				{
					Node* entity = new Node();
					entity->AddChild<Node>();
					entity->AddChild<BoxCollider2D>();
					entity->AddChild<BoxCollider2D>()->Trigger = true;
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
				m_cameraController = false;
			}
			
			ImGui::EndMenuBar();
			
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -30), false, ImGuiWindowFlags_HorizontalScrollbar);
			
			static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			
			for(int i = 0; i < Level::Get()->NodeCount; ++i)
			{
				bool nodeClicked = false;
				Node* entity = Level::Get()->Nodes[i][0];
				ImGuiTreeNodeFlags nodeFlags = baseFlags;
				if(IsSelected(entity))
				{
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				}
				
				nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "%s", entity->Name);
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
			ImGui::EndChild();
			ImGui::End();
		}
	}
	
	void ImGuiEditor::ShowDetails()
	{
		Node* node = m_selectedEntities[m_selectedEntities.size() - 1];
		
		node->Debug();
		
		ImGui::SetNextWindowSize(ImVec2(DETAILS_WINDOW_SIZE.X + DETAILS_WINDOW_PADDING.X, DETAILS_WINDOW_SIZE.Y + DETAILS_WINDOW_PADDING.Y));
		ImGui::SetNextWindowPos(ImVec2(m_editorWindowPos.x + (m_editorWindowSize.x), m_editorWindowPos.y));
		ImGui::Begin("Details");
		//put detail information here...
		
		float* pos[2] = { &node->Transform[0][3], &node->Transform[1][3] };

		// hack scale. will not modify scale when adjusted. TODO: change this to a different method that uses getters and setters rather than a pointer
		Vec2 nodeScale = node->GetScale();
		float* scale[2] = { &nodeScale.X, &nodeScale.Y };
		
		ImGui::DragFloat2("Position", *pos, 0.01f);
		
		ImGui::DragFloat2("Scale", *scale, 0.01f);
		
		// same hacky impl for rotation as we need to extract

		float rotation = node->GetRotation();

		ImGui::DragFloat("Rotation", &rotation, 1.0f, -180, 180);
		
		ImGui::Separator();
		
		//Render component..
		//currently hard coded inputs in future should be automatic depending on the variable type
		
		if(node->GetChild<Sprite>())
		{
			std::string path = node->GetChild<Sprite>()->Texture->GetFilePath();
			if (ImGui::Button("Change texture"))
			{
				
				if(path != "")
					m_filePath = path;
				
				Application::Get().AddPostUpdateCallback([&]() { Application::Get().PushOverlay(new ImGuiFileExplorer(
																													  m_filePath, [&](){
																														  m_selectedEntities[m_selectedEntities.size() - 1]->GetChild<Sprite>()->Texture->Load(m_filePath);
																													  } ));});
				
			}
			
			ImGui::Text(m_filePath.c_str());
			
			//Displaying texture on UI
			
			VulkanRenderer& renderer = (VulkanRenderer&)Application::Get().GetRenderer();
			VulkanTexture* vulkanTex = (VulkanTexture*)m_selectedEntTexture.get();
			
			if (m_selectedEntTexture->GetFilePath() != path && std::filesystem::exists(path))
			{
				m_selectedEntTexture->Load(path);
				m_selectedEntTextureID = (ImTextureID)ImGui_ImplVulkan_AddTexture(renderer.GetTexSampler(), vulkanTex->GetImageView(), VK_IMAGE_LAYOUT_UNDEFINED);
			}
			
			
			//determine correct aspect ration for the width given
			float aspectRatio = (float)m_selectedEntTexture->GetWidth() / (float)m_selectedEntTexture->GetHeight();
			
			if (m_selectedEntTextureID != nullptr)
			{
				ImGui::Image(m_selectedEntTextureID, { DETAILS_WINDOW_SIZE.X, DETAILS_WINDOW_SIZE.X * aspectRatio }, ImVec2{ 0, -1 }, ImVec2{ 1, 0 });
			}
		}
		if (node->GetChild<SubSprite>())
		{
			std::string path = node->GetChild<SubSprite>()->GetSubTexture()->GetFilePath();
			
			SubSprite* subSprite = node->GetChild<SubSprite>();
			float* coord[2] = { &subSprite->GetSubTexture()->GetCellCoords().X, &subSprite->GetSubTexture()->GetCellCoords().Y };
			
			float* size[2] = { &subSprite->GetSubTexture()->GetCellSize().X, &subSprite->GetSubTexture()->GetCellSize().Y };
			
			ImGui::DragFloat2("Sprite Coordinate", *coord, 1);
			
			ImGui::DragFloat2("Cell Size", *size, 1);
			
			if (ImGui::Button("Change texture"))
			{
				std::string path = node->GetChild<SubSprite>()->GetSubTexture()->GetFilePath();
				if(path != "")
					m_filePath = path;
				//kinda messy, but need to modify this layer stack after we finish itterating over them
				Application::Get().AddPostUpdateCallback([&](){Application::Get().PushOverlay(new ImGuiFileExplorer(m_filePath, [&](){m_selectedEntities
																															[m_selectedEntities.size() - 1]->GetChild<SubSprite>()->SetSubTexture(SubTexture::Create(m_filePath, Vec2(0,0), Vec2(32,32)));} ));});
				
			}
			ImGui::SameLine();
			if (ImGui::Button("Set Coords"))
			{
				subSprite->GetSubTexture()->SetTexCoords(subSprite->GetSubTexture()->GetCellCoords(), subSprite->GetSubTexture()->GetCellSize());
			}
			
			//storing start cursor pos before drawing the image, used to overlay buttons ontop of it later
			ImVec2 startCursorPos = ImGui::GetCursorPos();
			
			//Display tex as UI image
			VulkanRenderer& renderer = (VulkanRenderer&)Application::Get().GetRenderer();
			VulkanTexture* vulkanTex = (VulkanTexture*)m_selectedEntTexture.get();
			
			if (m_selectedEntTexture->GetFilePath() != path && std::filesystem::exists(path))
			{
				m_selectedEntTexture->Load(path);
				m_selectedEntTextureID = (ImTextureID)ImGui_ImplVulkan_AddTexture(renderer.GetTexSampler(), vulkanTex->GetImageView(), VK_IMAGE_LAYOUT_UNDEFINED);
			}
			
			
			
			
			//determine correct aspect ration for the width given
			float aspectRatio = (float)m_selectedEntTexture->GetWidth() / (float)m_selectedEntTexture->GetHeight();
			
			if (m_selectedEntTextureID != nullptr)
			{
				ImGui::Image(m_selectedEntTextureID, { DETAILS_WINDOW_SIZE.X, DETAILS_WINDOW_SIZE.X * aspectRatio }, ImVec2{ 0, -1 }, ImVec2{ 1, 0 });
			}
			
			//Overlaying buttons ontop of the image to select the cell we want
			ImVec2 buttonSize = ImVec2 (DETAILS_WINDOW_SIZE.X / subSprite->GetSubTexture()->GetTotalCells().X, (DETAILS_WINDOW_SIZE.X * aspectRatio) / subSprite->GetSubTexture()->GetTotalCells().Y);
			
			int buttonID = 0;
			
			for (int x = 0; x < subSprite->GetSubTexture()->GetTotalCells().X; ++x)
			{
				for (int y = 0; y < subSprite->GetSubTexture()->GetTotalCells().Y; ++y)
				{
					ImGui::SetCursorPos(ImVec2(startCursorPos.x + (x * buttonSize.x), startCursorPos.y + (y * buttonSize.y)));
					char buttonName[10];
					sprintf(buttonName, "%d Y:%d", x, y);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 0.1f));
					ImGui::PushID(buttonID);
					if (ImGui::Button("", buttonSize))
					{
						subSprite->GetSubTexture()->SetTexCoords(Vec2((float)x,(float)y), subSprite->GetSubTexture()->GetCellSize());
					}
					ImGui::PopStyleColor();
					ImGui::PopID();
					buttonID++;
				}
			}
			
		}
		ImGui::End();
	}
	
	
	
	void ImGuiEditor::UpdateSelectedEntity()
	{
		Vec2 mouseWorldPoint =  Level::Get()->MainCamera()->ScreenToWorld(Input::GetMousePosition());
		
		if(Input::IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			//Not sure about how nested this gets..
			//when editor is open if we click find the box collider where the world mouse position that click intersects
			
			for (int i = 0; i < Level::Get()->NodeCount; ++i)
			{
				Node* node = Level::Get()->Nodes[i][0];
				BoxCollider2D* collider = node->GetChild<BoxCollider2D>();
				if (node->GetChild<BoxCollider2D>())
				{
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
			if (e->ID == entity->ID)
				return true;
		}
		return false;
	}
	
}