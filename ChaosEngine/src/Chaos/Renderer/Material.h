/* date = April 24th 2021 7:25 am */

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/Renderer/Texture.h"

namespace Chaos
{
	class Material
	{
		public:
		virtual Vec4 GetColour() = 0;
		virtual void SetColour(Vec4 colour) = 0;
		
		virtual std::string GetName() = 0;
		virtual void SetName(std::string name) = 0;
		
		static Material* Create(std::string name, std::string fragShaderPath, std::string vertShaderPath);
		static Material* Get(std::string name);
	};
}

#endif //_MATERIAL_H
