
#pragma once
#include "RendererAPI.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec4.h"
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

		virtual void DrawQuad(Vec2& position, Vec2& scale, Ref<Texture> texture) = 0;
		virtual void DrawQuad(Vec2& position, Vec2& scale, Vec4& colour, Ref<Texture> texture)  = 0;
		virtual void DrawQuad(Vec2& position, Vec2& scale, Vec4& colour, Ref<Texture> texture, float tilingFactor) = 0;
		virtual void DrawQuad(Vec2& position, Vec2& scale, Ref<Texture> texture, float tilingFactor) = 0;
		virtual void DrawFrame() = 0;
		virtual void WindowResized() = 0;

	private:
		static RendererAPI sRendererAPI;		
	};
}