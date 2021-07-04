/* date = June 29th 2021 7:16 pm */

#ifndef _MESH_RENDERER_H
#define _MESH_RENDERER_H

#include "Chaos/Nodes/Node.h"

namespace Chaos
{
	struct RenderObject;
	class MeshRenderer : public Node
	{
		public:
		MeshRenderer();
		
		void OnUpdate(float delta) override;
		
		RenderObject* pRenderObject;
	};
}

#endif //_MESH_RENDERER_H
