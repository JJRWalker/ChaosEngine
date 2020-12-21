#include "chaospch.h"
#include "BoxCollider2D.h"
#include "Chaos/Entity/Entity.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/DataTypes/Vec4.h"

namespace Chaos
{
	//Overridden from base Collider, handles all collision detection with [any collider type] -> box colliders
	bool BoxCollider2D::CollideWith(Collider& other)
	{
		bool hit = false;
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
					hit = true;
				}
			}break;
			case ColliderType::LINE:
			{
				
			}break;
			case ColliderType::CIRCLE:
			{
				CircleCollider2D collider = (CircleCollider2D&)other;
				
				Vec3 circleDistance = Vec3(0,0,0);
				
				circleDistance.X = abs((other.GetEntity()->GetPosition().X + other.Offset().X) - (GetEntity()->GetPosition().X + m_offset.X));
				
				circleDistance.Y = abs((other.GetEntity()->GetPosition().Y + other.Offset().Y) - (GetEntity()->GetPosition().Y + m_offset.Y));
				
				//LOGCORE_TRACE("{0} {1}", circleDistance.X, circleDistance.Y);
				
				if (circleDistance.X > (m_extents.Width + collider.GetRadius()))
				{
					hit = false;
					break;
				}
				
				if (circleDistance.Y > (m_extents.Height + collider.GetRadius()))
				{
					hit = false;
					break;
				}
				
				if (circleDistance.X <= (m_extents.Width))
				{
					hit = true;
					break;
				}
				if (circleDistance.Y <= (m_extents.Height))
				{
					hit = true;
					break;
				}
				
				float cornerDistSq = pow((circleDistance.X - m_extents.Width), 2) + pow((circleDistance.Y - m_extents.Height), 2); 
				
				
				hit = cornerDistSq <= pow(collider.GetRadius(), 2);
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
			
			return true;
		}
		
		return false;
	}
	
	bool BoxCollider2D::PointCollision(Vec2& point)
	{
		return (point.X > (GetEntity()->GetPosition().X - (m_bounds.Width / 2)) && point.X <(GetEntity()->GetPosition().X + (m_bounds.Width / 2)) && point.Y > (GetEntity()->GetPosition().Y - (m_bounds.Height / 2)) && point.Y <(GetEntity()->GetPosition().Y + (m_bounds.Height / 2)));
	}
	
	void BoxCollider2D::Debug()
	{
		//find all the points of the box collider
		Vec2 extentPoints[4] = {{GetEntity()->GetPosition().X - m_extents.X, GetEntity()->GetPosition().Y - m_extents.Y},
			{GetEntity()->GetPosition().X - m_extents.X, GetEntity()->GetPosition().Y + m_extents.Y},
			{GetEntity()->GetPosition().X + m_extents.X, GetEntity()->GetPosition().Y + m_extents.Y},
			{GetEntity()->GetPosition().X + m_extents.X, GetEntity()->GetPosition().Y - m_extents.Y}};
		
		Renderer& renderer = Application::Get().GetRenderer();
		Vec4 debugColour = { 1.0f, 0.6f, 0.0f, 0.88f };
		
		//connect those points with a line
		//TODO: make some predefined debug render order somewhere.. currently 1001 is the working debug layer
		renderer.DrawLine(extentPoints[0], extentPoints[1], debugColour, 0.05f, 1001);
		renderer.DrawLine(extentPoints[1], extentPoints[2], debugColour, 0.05f, 1001);
		renderer.DrawLine(extentPoints[2], extentPoints[3], debugColour, 0.05f, 1001);
		renderer.DrawLine(extentPoints[3], extentPoints[0], debugColour, 0.05f, 1001);
		
	}
	
}