/* date = January 30th 2021 1:58 pm */

#ifndef _SPRITE_H
#define _SPRITE_H

#include "Chaos/DataTypes/Vec4.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Nodes/Node.h"

namespace Chaos
{
	class Sprite : public Node
	{
		public:
		Sprite();
		
		void Update(float delta) override;
		
		public:
		Ref<Texture> Texture = Texture::GetBlank();	//Setting default texture (loads blank)
		Vec4 Colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}

#endif //_SPRITE_H
