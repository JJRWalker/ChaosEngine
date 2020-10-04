#include "chaospch.h"
#include "LineCollider2D.h"
#include "Chaos/Entity/Entity.h"

namespace Chaos
{
	bool LineCollider2D::CollideWith(Collider& other)
	{
		switch (other.GetType())
		{
		case ColliderType::BOX2D:
		{

		}break;
		case ColliderType::LINE:
		{

		}break;

		}
		return false;
	}
}