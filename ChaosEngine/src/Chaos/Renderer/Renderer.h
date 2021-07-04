
#pragma once
#include "RendererAPI.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec3.h"
#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/Events/ApplicationEvent.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

namespace Chaos
{
	class Window;
	class Camera;
	
	// EXPANSION: should just need to add an additional float array here
	// Be sure to add that array to all shaders too! This kinda sucks but has to happen right now. Otherwise nothing will be rendered.
	// POTENTIAL TODO: late in development, some kind of auto shader layout generation before compilation.
	struct ShaderObjectData
	{
		float Transform[16];
		float DataArray1[16]; // for engine use
		float CustomDataArray1[16]; // for user use
	};
	
	struct RenderObject
	{
		Mesh* pMesh = nullptr;
		Material* pMaterial = nullptr;
		Texture* pTexture = Texture::GetBlank();
		ShaderObjectData ShaderData;
		uint32_t RenderID = 0;
		bool RenderOneTime = false;
	};
	
	
	struct LightingObjectData
	{
		float Transform[16];
		float DataArray1[16];
	};
	
	
	class Renderer							
	{
		friend class ImGuiLayer;
		public:
		
		static Renderer* Create(Window* window);
		static RendererAPI::API GetAPI();
		
		virtual void Init() = 0;
		
		// returns a render object pointer allowing you to update the transform from render component
		virtual RenderObject* AddQuad(float transform[16], Material* mat) = 0;
		virtual RenderObject* AddRenderable(RenderObject* toAdd) = 0;
		virtual void RemoveRenderable(RenderObject* toRemove) = 0;
		
		virtual LightingObjectData* AddLight(float transform[16]) = 0;
		virtual void RemoveLight(LightingObjectData* light) = 0;
		
		virtual void DrawLine(Vec2& startPoint, Vec2& endPoint, Vec4& colour, float weight, float renderOrder) = 0;
		
		
		virtual void DrawFrame() = 0;
		virtual bool OnWindowResized(WindowResizeEvent& e) = 0;
		virtual bool HasTexture(char* filePath, Ref<Texture> outTexture) = 0;
		virtual bool HasTexture(std::string name, Texture** outTexture = nullptr) = 0;
		
		virtual void InitImgui() = 0;
		
		virtual void SetVSync(bool state) = 0;
		virtual bool GetVSync() = 0;
		
		virtual void* GetImguiEditorPanelTextureID() = 0;
		
		private:
		static RendererAPI s_rendererAPI;
	};
}