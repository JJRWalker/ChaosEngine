#pragma once

#include <Chaos/Core/Core.h>


namespace Chaos
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void SetFilePath(const char* path) const = 0;
		virtual const char* GetFilePath() const = 0;

		virtual void SetWidth(uint32_t width) = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual void SetHeight(uint32_t height) = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual int GetSize() const = 0;

		static Ref<Texture> Create(const char* path);
	};
}