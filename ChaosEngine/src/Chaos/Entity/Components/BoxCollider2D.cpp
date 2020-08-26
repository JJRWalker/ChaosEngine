#include "chaospch.h"
#include "BoxCollider2D.h"
#include "Chaos/Entity/Entity.h"

namespace Chaos
{
	//Overridden from base Collider, handles all collision detection with [any collider type] -> box colliders
	bool BoxCollider2D::CollideWith(Collider& other)
	{
		switch (other.GetType())
		{
		case ColliderType::BOX2D:
		{
			BoxCollider2D collider = (BoxCollider2D&)other;
			if (GetEntity()->GetTransform()->Position().X + m_offset.X < collider.GetEntity()->GetTransform()->Position().X + collider.Offset().X + collider.GetBounds().Width &&
				GetEntity()->GetTransform()->Position().X + m_offset.X + m_bounds.Width > collider.GetEntity()->GetTransform()->Position().X + collider.Offset().X &&
				GetEntity()->GetTransform()->Position().Y + m_offset.Y < collider.GetEntity()->GetTransform()->Position().Y + collider.Offset().Y + collider.GetBounds().Height &&
				GetEntity()->GetTransform()->Position().Y + m_offset.Y + m_bounds.Height > collider.GetEntity()->GetTransform()->Position().Y + collider.Offset().Y)
			{
				//LOGCORE_INFO("Collision detected {0} -> {1}", GetEntity()->GetName(), other.GetEntity()->GetName());
				return true;
			}
		}break;
		case ColliderType::LINE:
		{

		}break;

		}
		return false;
	}
}