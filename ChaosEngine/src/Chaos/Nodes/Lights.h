/* date = June 24th 2021 8:13 am */

#ifndef _LIGHTS_H
#define _LIGHTS_H

#include "Chaos/Nodes/Node.h"

namespace Chaos
{
	struct LightingObjectData;
	class PointLight : public Node
	{
		public:
		PointLight();
		~PointLight();
		
		void OnUpdate(float delta) override;
		void SetEnabled(bool state) override;
		
		void OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle);
		
		void SetColour(Vec3 colour);
		Vec3 GetColour();
		
		void SetIntensity(float intensity);
		float GetIntensity();
		
		void SetRange(float range);
		float GetRange();
		
		private:
		LightingObjectData* p_lightObject;
		
		const float DEFAULT_SHADER_VALUES_1[16] = 
		{
			1.0f, 1.0f, 1.0f, 5.0f,
			2.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	};
}

#endif //_LIGHTS_H
