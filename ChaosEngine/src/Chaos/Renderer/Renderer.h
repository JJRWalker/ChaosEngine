
#pragma once
#include "RendererAPI.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec3.h"
#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/Events/ApplicationEvent.h"
#include "Texture.h"
#include "SubTexture.h"

namespace Chaos
{
	struct RenderData
	{
		size_t TotalQuadsDrawn = 0;
		size_t NumOfDrawCalls = 0;
	};
	
	class Renderer							
	{
		friend class ImGuiLayer;
		friend class VulkanTexture;
		public:
		
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		
		static Renderer* Create();
		
		virtual void DrawQuad(Vec3& position, Vec2& scale, Ref<Texture> texture) = 0;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec4& colour, Ref<Texture> texture)  = 0;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec4& colour, Ref<Texture> texture, float tilingFactor) = 0;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Ref<Texture> texture, float tilingFactor) = 0;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Ref<SubTexture> subTexture) = 0;
		
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec2& rotation, Ref<Texture> texture) = 0;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<Texture> texture) = 0;
		virtual void DrawQuad(float transform[4][4], Vec4& colour, Ref<Texture> texture) = 0;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<Texture> texture, float tilingFactor) = 0;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec2& rotation, Ref<SubTexture> subTexture) = 0;
		virtual void DrawQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<SubTexture> subTexture) = 0;
		
		virtual void DrawLine(Vec2& startPoint, Vec2& endPoint, Vec4& colour, float weight, float renderOrder) = 0;
		
		//UI
		virtual void DrawScreenSpaceQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<Texture> texture, float tilingFactor) = 0;
		virtual void DrawScreenSpaceQuad(Vec3& position, Vec2& scale, Vec2& rotation, Vec4& colour, Ref<SubTexture> subTexture) = 0;
		
		virtual void DrawFrame() = 0;
		virtual bool OnWindowResized(WindowResizeEvent& e) = 0;
		virtual bool HasTexture(char* filePath, Ref<Texture> outTexture) = 0;
		virtual bool HasTexture(std::string filePath, Ref<Texture> outTexture) = 0;
		virtual RenderData& GetDebugInfo() = 0;
		
		private:
		static RendererAPI s_rendererAPI;		
	};
}