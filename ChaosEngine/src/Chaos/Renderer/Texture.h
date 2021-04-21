#pragma once

#include <Chaos/Core/Core.h>
#include <string>

namespace Chaos
{
	class Texture
	{
		public:
		virtual ~Texture() = default;
		
		virtual void Load(char* filePath) = 0;
		virtual void Load(std::string filePath) = 0;
		virtual void Unload() = 0;
		
		virtual std::string GetFilePath() const = 0;
		
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetSize() const = 0;
		
		static Ref<Texture> Create(std::string filePath);
		static Ref<Texture> Create();
		static Ref<Texture> GetBlank();
		private:
		static Ref<Texture> s_blankTexture;
	};
}