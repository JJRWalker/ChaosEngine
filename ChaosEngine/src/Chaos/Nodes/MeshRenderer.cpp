#include "chaospch.h"

#include "MeshRenderer.h"

#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"
namespace Chaos
{
	MeshRenderer::MeshRenderer(bool child) : Node(child)
	{
		Name = "Mesh-Renderer";
		Type = NodeType::MESH_RENDERER;
		
		pRenderObject = new RenderObject();
		pRenderObject->pMesh = Mesh::Create("./Assets/Meshes/monkey_smooth.obj");
		pRenderObject->pMaterial = Material::Get("textured-default");
		
		memset(pRenderObject->ShaderData.DataArray1, 0, sizeof(float) * 16);
		memset(pRenderObject->ShaderData.CustomDataArray1, 0, sizeof(float) * 16);
		
		Application::Get().GetRenderer().AddRenderable(pRenderObject);
		
		Vec4 colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		
		memcpy((void*)&pRenderObject->ShaderData.DataArray1, (void*)&colour, sizeof(float) * 4);
	}
	
	void MeshRenderer::OnUpdate(float delta)
	{
		memcpy((void*)&pRenderObject->ShaderData.Transform[0], (void*)&GetWorldTransform()[0], sizeof(float) * 16);
	}
}