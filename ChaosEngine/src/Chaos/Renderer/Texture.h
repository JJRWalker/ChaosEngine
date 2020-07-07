#pragma once

#include <Chaos/Core/Core.h>


namespace Chaos
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void Load(char* filePath) = 0;
		virtual void Unload() = 0;

		virtual char* GetFilePath() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetSize() const = 0;

		static Ref<Texture> Create(char* filePath);
		static Ref<Texture> Create();
		static Ref<Texture> GetBlank();
	private:
		static Ref<Texture> s_blankTexture;
	};
}