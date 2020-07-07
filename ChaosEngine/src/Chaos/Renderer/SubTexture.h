#pragma once
#include "Texture.h"
#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class SubTexture
	{
	public:
		virtual ~SubTexture() = default;

		virtual char* GetFilePath() const = 0;

		virtual const Vec2* GetTexCoords() const = 0;
		virtual const Ref<Texture> GetMainTexture() const = 0;

		static Ref<SubTexture> Create(char* path, Vec2 Coordinates, Vec2 size);
		static Ref<SubTexture> Create(Ref<Texture> mainTexture, Vec2 coordinates, Vec2 size);
	};
}