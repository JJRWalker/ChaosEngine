
#pragma once
#include "RendererAPI.h"
#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec3.h"
#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/Events/ApplicationEvent.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "SubTexture.h"

namespace Chaos
{
	class Window;
	class Camera;
	
	struct RenderObject
	{
		Mesh* pMesh = nullptr;
		Material* pMaterial = nullptr;
		Texture* pTexture = Texture::GetBlank();
		float Transform[16];
		float ShaderDataArray1[16];
		uint32_t RenderID = 0;
	};
	
	
	class Renderer							
	{
		friend class ImGuiLayer;
		public:
		
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		
		static Renderer* Create(Window* window);
		
		virtual void Init() = 0;
		
		// returns a render object pointer allowing you to update the transform from render component
		virtual RenderObject* AddQuad(float transform[16], Material* mat) = 0;
		virtual void AddRenderable(RenderObject& toAdd) = 0;
		virtual void RemoveRenderable(RenderObject* toRemove) = 0;
		
		virtual void DrawLine(Vec2& startPoint, Vec2& endPoint, Vec4& colour, float weight, float renderOrder) = 0;
		
		
		virtual void DrawFrame() = 0;
		virtual bool OnWindowResized(WindowResizeEvent& e) = 0;
		virtual bool HasTexture(char* filePath, Ref<Texture> outTexture) = 0;
		virtual bool HasTexture(std::string name, Texture** outTexture = nullptr) = 0;
		
		virtual void InitImgui() = 0;
		virtual void SetCamera(Camera* camera) = 0;
		
		virtual void SetVSync(bool state) = 0;
		virtual bool GetVSync() = 0;
		
		private:
		static RendererAPI s_rendererAPI;		
	};
}