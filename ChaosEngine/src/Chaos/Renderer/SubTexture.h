#pragma once
#include "Texture.h"
#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class SubTexture
	{
		public:
		virtual ~SubTexture() = default;
		
		virtual std::string GetFilePath() const = 0;
		
		virtual void SetTexCoords(Vec2 coordinates, Vec2 cellSize) = 0; 
		
		virtual const Vec2* GetTexCoords() const = 0;
		virtual Vec2& GetCellSize() = 0;
		virtual Vec2& GetCellCoords() = 0;
		virtual Vec2& GetTotalCells() = 0;
		virtual const Ref<Texture> GetMainTexture() const = 0;
		
		static Ref<SubTexture> Create(std::string path, Vec2 Coordinates, Vec2 size);
		static Ref<SubTexture> Create(Ref<Texture> mainTexture, Vec2 coordinates, Vec2 size);
	};
}