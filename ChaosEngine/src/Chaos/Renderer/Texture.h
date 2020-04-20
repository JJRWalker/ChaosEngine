#pragma once

#include <Chaos/Core/Core.h>


namespace Chaos
{
	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void SetFilePath(const char* path) const = 0;
		virtual const char* GetFilePath() const = 0;

		virtual void SetWidth(uint32_t width) = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual void SetHeight(uint32_t height) = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual int GetSize() const = 0;
		virtual void* GetData() const = 0;
		inline virtual void SetData(void* data) = 0;
		inline virtual float GetTilingFactor() const = 0;

		static Texture* Create(const char* path, float tilingFactor);
	};
}