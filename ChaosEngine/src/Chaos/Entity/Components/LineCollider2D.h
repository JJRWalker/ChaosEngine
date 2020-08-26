#pragma once
#include "Collider.h"
#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	class LineCollider2D : public Collider
	{
	public:
		LineCollider2D() { m_type = ColliderType::LINE; }
		LineCollider2D(Vec2 lineStart, Vec2 lineEnd) : m_lineStart(lineStart), m_lineEnd(lineEnd) { m_type = ColliderType::LINE; }


		bool CollideWith(Collider& other) override;

		void SetLineStart(Vec2 start) { m_lineStart = start; }
		Vec2 GetLineStart() { return  m_lineStart; }
		void SetLineEnd(Vec2 end) { m_lineEnd = end; }
		Vec2 GetLineEnd() { return m_lineEnd; }
	private:
		Vec2 m_lineStart = Vec2::Zero();
		Vec2 m_lineEnd = Vec2::Zero();
	};
}