#include "chaospch.h"

#include "VulkanSubTexture.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Chaos/Core/Application.h"

namespace Chaos
{
	VulkanSubTexture::VulkanSubTexture(std::string filePath, Vec2 coordinates, Vec2 size) : m_filePath(filePath), m_coordinates(coordinates)
	{
		//Check if the renderer already has the texture in it's list of textures to bind, if it does it will set the main texture reference to that texture
		//if it doesn't it will create a new ref to a new texture 
		if (!Application::Get().GetRenderer().HasTexture(filePath, m_mainTexture))
		{
			m_mainTexture = CreateRef<VulkanTexture>(filePath);
		}
		
		//Setting the tex coords based on the size of the main texture and the x and y coords given
		Vec2 min = { (coordinates.X * size.X) / m_mainTexture->GetWidth(), (coordinates.Y * size.Y) / m_mainTexture->GetHeight() };
		Vec2 max = { ((coordinates.X + 1) * size.X) / m_mainTexture->GetWidth(), ((coordinates.Y + 1) * size.Y) / m_mainTexture->GetHeight() };
		
		m_texCoords[0].X = min.X;
		m_texCoords[0].Y = min.Y;
		
		m_texCoords[1].X = max.X;
		m_texCoords[1].Y = min.Y;
		
		m_texCoords[2].X = max.X;
		m_texCoords[2].Y = max.Y;
		
		m_texCoords[3].X = min.X;
		m_texCoords[3].Y = max.Y;
		
		m_cellSize = size;
	}
	VulkanSubTexture::VulkanSubTexture(Ref<Texture> mainTexture, Vec2 coordinates, Vec2 size) : m_filePath(mainTexture->GetFilePath()), m_coordinates(coordinates)
	{
		m_mainTexture = std::static_pointer_cast<VulkanTexture>(mainTexture);
		
		//Setting the tex coords based on the size of the main texture and the x and y coords given
		Vec2 min = { (coordinates.X * size.X) / m_mainTexture->GetWidth(), (coordinates.Y * size.Y) / m_mainTexture->GetHeight() };
		Vec2 max = { ((coordinates.X + 1) * size.X) / m_mainTexture->GetWidth(), ((coordinates.Y + 1) * size.Y) / m_mainTexture->GetHeight() };
		
		m_texCoords[0].X = min.X;
		m_texCoords[0].Y = min.Y;
		
		m_texCoords[1].X = max.X;
		m_texCoords[1].Y = min.Y;
		
		m_texCoords[2].X = max.X;
		m_texCoords[2].Y = max.Y;
		
		m_texCoords[3].X = min.X;
		m_texCoords[3].Y = max.Y;
		m_cellSize = size;
	}
	
	void VulkanSubTexture::SetTexCoords(Vec2 coordinates, Vec2 cellSize)
	{
		//Setting the tex coords based on the size of the main texture and the x and y coords given
		Vec2 min = { (coordinates.X * cellSize.X) / m_mainTexture->GetWidth(), (coordinates.Y * cellSize.Y) / m_mainTexture->GetHeight() };
		Vec2 max = { ((coordinates.X + 1) * cellSize.X) / m_mainTexture->GetWidth(), ((coordinates.Y + 1) * cellSize.Y) / m_mainTexture->GetHeight() };
		
		m_texCoords[0].X = min.X;
		m_texCoords[0].Y = min.Y;
		
		m_texCoords[1].X = max.X;
		m_texCoords[1].Y = min.Y;
		
		m_texCoords[2].X = max.X;
		m_texCoords[2].Y = max.Y;
		
		m_texCoords[3].X = min.X;
		m_texCoords[3].Y = max.Y;
	}
}