#include <Chaos/Entity/Components/CircleCollider2D.h>

namespace Chaos
{
	CircleCollider2D::CircleCollider2D()
	{
		m_type = ColliderType::CIRCLE;
	}
	
	bool CircleCollider2D::CollideWith(Collider& other)
	{
		bool hit = false;
		switch(other.GetType())
		{
			case ColliderType::BOX2D:
			{
				BoxCollider2D collider = (BoxCollider2D&)other;
				
				Vec3 circleDistance = Vec3(0,0,0);
				
				circleDistance.X = abs((other.GetEntity()->GetPosition().X + other.Offset().X) - (GetEntity()->GetPosition().X + m_offset.X));
				
				circleDistance.Y = abs((other.GetEntity()->GetPosition().Y + other.Offset().Y) - (GetEntity()->GetPosition().Y + m_offset.Y));
				
				LOGCORE_TRACE("{0} {1}", circleDistance.X, circleDistance.Y);
				
				if (circleDistance.X > (collider.GetExtents().Width + m_radius))
				{
					hit = false;
					break;
				}
				
				if (circleDistance.Y > (collider.GetExtents().Height + m_radius))
				{
					hit = false;
					break;
				}
				
				if (circleDistance.X <= collider.GetExtents().Width)
				{
					hit = true;
					break;
				}
				if (circleDistance.Y <= collider.GetExtents().Height)
				{
					hit = true;
					break;
				}
				
				float cornerDistSq = pow((circleDistance.X - collider.GetExtents().Width), 2) + pow((circleDistance.Y - collider.GetExtents().Height), 2); 
				
				
				hit = cornerDistSq <= pow(m_radius, 2);
				
			}break;
			
			case ColliderType::LINE:
			{
			}break;
			
			case ColliderType::CIRCLE:
			{
				CircleCollider2D collider = (CircleCollider2D&)other;
				
				Vec2 circleDistance;
				circleDistance.X = abs((other.GetEntity()->GetPosition().X + other.Offset().X) - (GetEntity()->GetPosition().X + m_offset.X));
				
				circleDistance.Y = abs((other.GetEntity()->GetPosition().Y + other.Offset().Y) - (GetEntity()->GetPosition().Y + m_offset.Y));
				
				if (circleDistance.Magnitude() < m_radius || circleDistance.Magnitude() < collider.GetRadius())
					hit = true;
				
			}break;
		}
		if (hit)
		{
			if (!m_trigger && !other.IsTrigger())
			{
				GetEntity()->ColliderHit(this, &other);
				other.GetEntity()->ColliderHit(&other, this);
			}
			else
			{
				GetEntity()->TriggerHit(this, &other);
				other.GetEntity()->TriggerHit(&other, this);
			}
		}
		return hit;
	}
}