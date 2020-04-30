
#pragma once
#include "RendererAPI.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Texture.h"

namespace Chaos
{
	class Renderer
	{
		friend class ImGuiLayer;
		friend class VulkanTexture;
	public:

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static Renderer* Create();

		virtual void DrawQuad(Vec2& position, Vec2& scale, Texture* texture) = 0;
		virtual void DrawFrame() = 0;
		virtual bool WaitIdle() = 0;
		virtual void WindowResized() = 0;

	private:
		static RendererAPI sRendererAPI;		
	};
}