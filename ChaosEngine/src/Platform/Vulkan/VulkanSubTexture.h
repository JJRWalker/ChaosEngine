#pragma once
#include "Chaos/Renderer/SubTexture.h"
#include "VulkanTexture.h"

namespace Chaos
{
	class VulkanSubTexture : public SubTexture
	{
		public:
		//Size here corrisponds to the size of each sprite on the sprite sheet, or texture on the subtexture, e.g. if it is the size of the sprite coord [1,1] will be the entire sprite
		VulkanSubTexture(std::string filePath, Vec2 coordinates, Vec2 size);
		VulkanSubTexture(Ref<Texture> mainTexture, Vec2 coordinates, Vec2 size);
		
		virtual ~VulkanSubTexture() { m_mainTexture->Unload(); };

		virtual void Load(Ref<Texture> mainTex, Vec2 coordinates, Vec2 size) override {};
		virtual void Unload() override {};

		
		virtual std::string GetFilePath() const override { return m_filePath; }
		
		virtual void SetTexCoords(Vec2 coordinates, Vec2 cellSize) override;
		
		
		virtual const Vec2* GetTexCoords() const override { return m_texCoords; }
		virtual Vec2& GetCellSize() override {return m_cellSize;}
		virtual Vec2& GetCellCoords() override {return m_coordinates;}
		virtual Vec2& GetTotalCells() override { m_totalCells = Vec2(m_mainTexture->GetWidth() / m_cellSize.X, m_mainTexture->GetHeight() / m_cellSize.Y); return m_totalCells;}
		virtual const Ref<Texture> GetMainTexture() const override { return m_mainTexture; }
		
		private:
		std::string m_filePath;
		Vec2 m_cellSize;
		Vec2 m_coordinates;
		Vec2 m_texCoords[4];
		Vec2 m_totalCells;
		Ref<VulkanTexture> m_mainTexture;
	};
}