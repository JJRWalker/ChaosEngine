#include "chaospch.h"

#include "Sprite.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Renderer/Material.h"

// debug GUI
#include "Chaos/Serialisation/FileUtils.h"
#include "Chaos/Debug/ImGuiEditor.h"
#include <filesystem>

namespace Chaos
{
	// SPRITE
	Sprite::Sprite(bool child) : Node(child)
	{
		Name = "Sprite";
		
		p_renderObject = Application::Get().GetRenderer().AddQuad(GetWorldTransform(), Material::Get("textured-default"));
		memset(p_renderObject->ShaderData.DataArray1, 0, sizeof(float) * 16);
		memset(p_renderObject->ShaderData.CustomDataArray1, 0, sizeof(float) * 16);
		SetColour(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	
	
	Sprite::Sprite(Vec2 position, bool child) : Node(child)
	{
		Name = "Sprite";
		
		SetPosition(position);
		
		p_renderObject = Application::Get().GetRenderer().AddQuad(GetWorldTransform(), Material::Get("textured-default"));
		memset(p_renderObject->ShaderData.DataArray1, 0, sizeof(float) * 16);
		memset(p_renderObject->ShaderData.CustomDataArray1, 0, sizeof(float) * 16);
		SetColour(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	
	
	Sprite::Sprite(Vec3 position, bool child) : Node(child)
	{
		Name = "Sprite";
		
		SetPosition(position);
		
		p_renderObject = Application::Get().GetRenderer().AddQuad(GetWorldTransform(), Material::Get("textured-default"));
		memset(p_renderObject->ShaderData.DataArray1, 0, sizeof(float) * 16);
		memset(p_renderObject->ShaderData.CustomDataArray1, 0, sizeof(float) * 16);
		SetColour(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	
	
	Sprite::~Sprite()
	{
		Application::Get().GetRenderer().RemoveRenderable(p_renderObject);
	}
	
	
	void Sprite::OnUpdate(float delta)
	{
		if (UpdateType == SpriteUpdateType::DYNAMIC)
			// copy current global transform to the render object
			memcpy((void*)&p_renderObject->ShaderData.Transform[0], (void*)&GetWorldTransform()[0], sizeof(float) * 16);
	}
	
	
	void Sprite::SetEnabled(bool state)
	{
		if (Enabled == state)
			return;
		Enabled = state;
		if (!state)
			Application::Get().GetRenderer().RemoveRenderable(p_renderObject);
		else
			Application::Get().GetRenderer().AddRenderable(p_renderObject);
	}
	
	
	void Sprite::OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle)
	{
		ImTextureID* imguiEditorTextureID = nullptr;
		if (editorImageHandle != nullptr)
			imguiEditorTextureID = (ImTextureID*)editorImageHandle;
		
		std::string path = p_renderObject->pTexture->GetFilePath();
		
		ImGui::Text(path.c_str());
		
		if (ImGui::Button("Change texture"))
		{
			std::string out;
			if (FileUtils::OpenFileDialogue(out))
			{
				if (out != path)
				{
					p_renderObject->pTexture->Load(out);
				}
			}
		}
		
		
		//Displaying texture on UI
		
		if (editorTexture->GetFilePath() != path && std::filesystem::exists(path))
		{
			editorTexture->Load(path);
		}
		
		//determine correct aspect ration for the width given
		float aspectRatio = (float)editorTexture->GetWidth() / (float)editorTexture->GetHeight();
		
		if (imguiEditorTextureID != nullptr)
		{
			ImGui::Image(imguiEditorTextureID, { DETAILS_WINDOW_SIZE.X, DETAILS_WINDOW_SIZE.X * aspectRatio }, ImVec2{ 0, -1 }, ImVec2{ 1, 0 });
		}
	}
	
	
	void Sprite::SetMaterial(Material* mat) 
	{
		p_renderObject->pMaterial = mat; 
	}
	
	
	Material* Sprite::GetMaterial()
	{
		return p_renderObject->pMaterial;
	}
	
	
	void Sprite::SetTexture(Texture* tex)
	{
		p_renderObject->pTexture = tex;
	}
	
	
	Texture* Sprite::GetTexture()
	{
		return p_renderObject->pTexture;
	}
	
	
	void Sprite::SetColour(Vec4 colour)
	{
		memcpy((void*)&p_renderObject->ShaderData.DataArray1, (void*)&colour, sizeof(float) * 4);
		
	}
	
	
	Vec4 Sprite::GetColour()
	{
		return Vec4(p_renderObject->ShaderData.DataArray1[0],
					p_renderObject->ShaderData.DataArray1[1], p_renderObject->ShaderData.DataArray1[2], p_renderObject->ShaderData.DataArray1[3]);
	}
	
	
	void Sprite::SetShaderFloatData(size_t index, float value)
	{
		p_renderObject->ShaderData.DataArray1[index] = value;
	}
	
	
	// SUB SPRITE
	SubSprite::SubSprite(bool child) : Sprite(child)
	{
		Name = "SubSprite";
		
		SetMaterial(Material::Get("subsprite-default"));
		SetTilingFactor(Vec2(1.0f, 1.0f));
	}
	
	
	SubSprite::SubSprite(Vec2 position, bool child) : Sprite(position, child)
	{
		Name = "SubSprite";
		
		SetMaterial(Material::Get("subsprite-default"));
		SetTilingFactor(Vec2(1.0f, 1.0f));
	}
	
	
	SubSprite::SubSprite(Vec3 position, bool child) : Sprite(position, child)
	{
		Name = "SubSprite";
		
		SetMaterial(Material::Get("subsprite-default"));
		SetTilingFactor(Vec2(1.0f, 1.0f));
	}
	
	
	void SubSprite::OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle)
	{
		ImTextureID* imguiEditorTextureID = nullptr;
		if (editorImageHandle != nullptr)
			imguiEditorTextureID = (ImTextureID*)editorImageHandle;
		
		std::string path = p_renderObject->pTexture->GetFilePath();
		
		ImGui::Text(path.c_str());
		
		if (ImGui::Button("Change texture"))
		{
			std::string out;
			if (FileUtils::OpenFileDialogue(out))
			{
				if (out != path)
				{
					p_renderObject->pTexture->Load(out);
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Set Coords"))
		{
			
		}
		
		//storing start cursor pos before drawing the image, used to overlay buttons ontop of it later
		ImVec2 startCursorPos = ImGui::GetCursorPos();
		
		//Display tex as UI image
		if (editorTexture->GetFilePath() != path && std::filesystem::exists(path))
		{
			editorTexture->Load(path);
		}
		
		
		//determine correct aspect ration for the width given
		float aspectRatio = (float)editorTexture->GetWidth() / (float)editorTexture->GetHeight();
		
		if (imguiEditorTextureID != nullptr)
		{
			ImGui::Image(imguiEditorTextureID, { DETAILS_WINDOW_SIZE.X, DETAILS_WINDOW_SIZE.X * aspectRatio }, ImVec2{ 0, -1 }, ImVec2{ 1, 0 });
		}
		
		
		//Overlaying buttons ontop of the image to select the cell we want
		ImVec2 buttonSize = ImVec2 (DETAILS_WINDOW_SIZE.X / GetTotalCells().X, (DETAILS_WINDOW_SIZE.X * aspectRatio) / GetTotalCells().Y);
		
		int buttonID = 0;
		
		for (int x = 0; x < GetTotalCells().X; ++x)
		{
			for (int y = 0; y < GetTotalCells().Y; ++y)
			{
				ImGui::SetCursorPos(ImVec2(startCursorPos.x + (x * buttonSize.x), startCursorPos.y + (y * buttonSize.y)));
				char buttonName[10];
				sprintf(buttonName, "%d Y:%d", x, y);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 0.1f));
				ImGui::PushID(buttonID);
				if (ImGui::Button("", buttonSize))
				{
					SetCoords(Vec2((float)x,(float)y));
				}
				ImGui::PopStyleColor();
				ImGui::PopID();
				buttonID++;
			}
		}
		
		
	}
	
	
	// not keen on this approach. Kinda removes visibility from what's actually going on.
	// not explicit that shader data array index 0 and 1 are used by the subsprite material
	// could be reused by the user and cause issues
	void SubSprite::SetCoords(Vec2 coords)
	{
		SetShaderFloatData(4, coords.X);
		SetShaderFloatData(5, coords.Y);
	}
	
	
	void SubSprite::SetTotalCells(Vec2 dimensions)
	{
		SetShaderFloatData(6, dimensions.X);
		SetShaderFloatData(7, dimensions.Y);
	}
	
	
	void SubSprite::SetTilingFactor(Vec2 tilingFactor)
	{
		SetShaderFloatData(8, tilingFactor.X);
		SetShaderFloatData(9, tilingFactor.Y);
	}
	
	
	Vec2 SubSprite::GetCoords()
	{
		return Vec2(p_renderObject->ShaderData.DataArray1[4], p_renderObject->ShaderData.DataArray1[5]);
	}
	
	
	Vec2 SubSprite::GetTotalCells()
	{
		return Vec2(p_renderObject->ShaderData.DataArray1[6], p_renderObject->ShaderData.DataArray1[7]);
	}
	
	
	Vec2 SubSprite::GetTilingFactor()
	{
		return Vec2(p_renderObject->ShaderData.DataArray1[8], p_renderObject->ShaderData.DataArray1[9]);
	}
	
	
	// UI SPRITE
	UISprite::UISprite(bool child) : Sprite(child)
	{
		Name = "UISprite";
		SetMaterial(Material::Get("ui-default"));
	}
}
