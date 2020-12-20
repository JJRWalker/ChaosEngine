/* date = December 6th 2020 4:04 pm */

#ifndef _CIRCLE_COLLIDER2_D_H
#define _CIRCLE_COLLIDER2_D_H
#include <Chaos/Entity/Components/Collider.h>

namespace Chaos
{
	class CircleCollider2D : public Collider
	{
		public:
		CircleCollider2D();
		
		bool CollideWith(Collider& other) override;
		
		void SetRadius(float radius) {m_radius = radius;}
		float GetRadius() {return m_radius;}
		
		private:
		float m_radius = 1.0f;
	};
}

#endif //_CIRCLE_COLLIDER2_D_H
