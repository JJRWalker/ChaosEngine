#include "chaospch.h"

#include "Node.h"
#include "Math.h"
#include "Colliders.h"

namespace Chaos
{
	Node::Node(bool child)
	{
		if (!child)
		{
			Level* level = Level::Get();
			ID = (uint32_t)level->NodeCount;
			level->Nodes[level->NodeCount][0] = this;
			level->NodeCount++;
		}
	}
	
	
	Node::~Node()
	{
		OnDestroy();
	}
	
	
	void Node::OnStart()
	{
	}
	
	
	void Node::OnUpdate(float delta)
	{
	}
	
	
	void Node::OnFixedUpdate(float delta)
	{
	}
	
	void Node::OnDestroy()
	{
	}
	
	void Node::Debug()
	{
	}
	
	
	float* Node::GetGlobalTransform()
	{
		memcpy((void*)&m_globalTransform[0], (void*)&Transform[0], sizeof(float) * 16);
		if (p_parent)
		{
			float* parentTransform = p_parent->GetGlobalTransform();
			for (int i = 0; i < 4; ++i)
			{
				m_globalTransform[i] += parentTransform[i];
			}
		}
		return &m_globalTransform[0];
	}
	
	
	Vec2 Node::GetPosition()
	{
		Vec2 pos =  Vec2(Transform[12], Transform[13]);
		if (p_parent)
			pos = pos + p_parent->GetPosition();
		return pos;
	}
	
	
	void Node::SetPosition(Vec2 position)
	{
		if (p_parent)
			position = position + p_parent->GetPosition();
		Transform[12] = position.X;
		Transform[13] = position.Y;
	}
	
	
	Vec3 Node::GetPosition3D()
	{
		Vec3 pos = Vec3(Transform[12], Transform[13], Transform[14]);
		if (p_parent)
			pos = pos + p_parent->GetPosition3D();
		return pos;
	}
	
	
	void Node::SetPosition(Vec3 position)
	{
		if (p_parent)
			position = position + p_parent->GetPosition3D();
		Transform[12] = position.X;
		Transform[13] = position.Y;
		Transform[14] = position.Z;
	}
	
	
	float Node::GetDepth()
	{
		return Transform[14];
	}
	
	
	void Node::SetDepth(float depth)
	{
		Transform[14] = depth;
	}
	
	
	void Node::Translate(Vec2 translation)
	{
		Transform[12] += translation.X;
		Transform[13] += translation.Y;
	}
	
	
	float Node::GetRotation()
	{
		float theta = atan2(Transform[4], Transform[0]);
		if (p_parent)
			theta += p_parent->GetRotation();
		return theta;
	}
	
	
	void Node::SetRotation(float rotation)
	{
		Vec2 scale = GetScale();
		
		float offset = 0;
		
		if (p_parent)
			offset = p_parent->GetRotation();
		
		rotation += offset;
		//rotation = rotation - (int)(rotation / PI) * PI;
		
		Transform[0] = cosf(rotation) * scale.X;
		Transform[1] = -sinf(rotation) * scale.X;
		Transform[4] = sin(rotation) * scale.Y;
		Transform[5] = cosf(rotation) * scale.Y;
	}
	
	
	void Node::Rotate(float theta)
	{
		float rotation = GetRotation();
		
		SetRotation(rotation + theta);
	}
	
	
	Vec2 Node::GetScale()
	{
		Vec2 right = Vec2(Transform[0], Transform[1]);
		Vec2 up = Vec2(Transform[4], Transform[5]);
		
		int rightMod = right.X + right.Y < 0 ? -1 : 1;
		int upMod = up.X + up.Y < 0 ? -1 : 1;
		
		Vec2 scale = Vec2(right.Magnitude(), up.Magnitude());
		
		if (p_parent)
			scale = scale * p_parent->GetScale();
		
		return scale;
	}
	
	
	void Node::SetScale(Vec2 scale)
	{
		Vec2 offset = Vec2();
		
		float rotation = GetRotation();
		
		if (p_parent)
			offset = p_parent->GetScale();
		
		Transform[0] = cosf(rotation) * scale.X;
		Transform[1] = -sinf(rotation) * scale.X;
		Transform[4] = sin(rotation) * scale.Y;
		Transform[5] = cosf(rotation) * scale.Y;
	}
	
	size_t Node::GetSize()
	{
		return sizeof(*this);
	}
	
	
	void Node::ColliderStay(Collider* self, Collider* other)
	{
	}
	
	
	void Node::ColliderEnter(Collider* self, Collider* other)
	{
	}
	
	
	void Node::ColliderExit(Collider* self, Collider* other)
	{
	}
	
	
	void Node::TriggerStay(Collider* self, Collider* other)
	{
	}
	
	
	void Node::TriggerEnter(Collider* self, Collider* other)
	{
	}
	
	
	void Node::TriggerExit(Collider* self, Collider* other)
	{
	}
}