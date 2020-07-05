#pragma once

#include <Chaos/Core/Core.h>


namespace Chaos
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void Load(const char* filePath) = 0;
		virtual void Unload() = 0;

		virtual const char* GetFilePath() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetSize() const = 0;

		static Ref<Texture> Create(const char* filePath);
		static Ref<Texture> Create();
		static Ref<Texture> GetBlank();
	private:
		static Ref<Texture> s_blankTexture;
	};
}