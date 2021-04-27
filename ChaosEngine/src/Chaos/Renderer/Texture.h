#pragma once

#include <Chaos/Core/Core.h>
#include <string>

namespace Chaos
{
	class Texture
	{
		public:
		virtual ~Texture() = default;
		
		virtual void Load(std::string filePath) = 0;
		virtual void Unload() = 0;
		
		virtual std::string GetFilePath() const = 0;
		
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetSize() const = 0;
		
		static Texture* Create(std::string filePath);
		static Texture* Create();
		static Texture* GetBlank();

		public:
		std::string Name = "blank";

		private:
		static Texture* s_blankTexture;
	};
}