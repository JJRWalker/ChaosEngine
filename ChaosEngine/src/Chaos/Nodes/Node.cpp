#include "chaospch.h"

#include "Node.h"
#include "Math.h"
#include "Colliders.h"

namespace Chaos
{
	Node::Node()
	{
		Level* level = Level::Get();
		level->Nodes[level->NodeCount][0] = this;
		ID = (uint32_t)level->NodeCount;
		level->NodeCount++;
	}
	
	
	Node::~Node()
	{
		Kill();
	}
	
	
	void Node::Init()
	{
	}
	
	
	void Node::Update(float delta)
	{
	}
	
	
	void Node::FixedUpdate(float delta)
	{
	}
	
	void Node::Kill()
	{
	}

	void Node::Debug()
	{
	}
	
	
	Vec2 Node::GetPosition()
	{
		Vec2 pos =  Vec2(Transform[3][0], Transform[3][1]);
		if (p_parent)
			pos = pos + p_parent->GetPosition();
		return pos;
	}
	
	
	void Node::SetPosition(Vec2 position)
	{
		if (p_parent)
			position = position + p_parent->GetPosition();
		Transform[3][0] = position.X;
		Transform[3][1] = position.Y;
	}
	
	
	Vec3 Node::GetPosition3D()
	{
		Vec3 pos = Vec3(Transform[3][0], Transform[3][1], Transform[3][2]);
		if (p_parent)
			pos = pos + p_parent->GetPosition3D();
		return pos;
	}
	
	
	void Node::SetPosition(Vec3 position)
	{
		if (p_parent)
			position = position + p_parent->GetPosition3D();
		Transform[3][0] = position.X;
		Transform[3][1] = position.Y;
		Transform[3][2] = position.Z;
	}
	
	
	float Node::GetDepth()
	{
		return Transform[3][2];
	}
	
	
	void Node::SetDepth(float depth)
	{
		Transform[3][2] = depth;
	}
	
	
	void Node::Translate(Vec2 translation)
	{
		Transform[3][0] += translation.X;
		Transform[3][1] += translation.Y;
	}
	
	
	float Node::GetRotation()
	{
		float theta = atan2(Transform[1][0], Transform[0][0]);
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
		
		Transform[0][0] = cosf(rotation) * scale.X;
		Transform[0][1] = -sinf(rotation) * scale.X;
		Transform[1][0] = sin(rotation) * scale.Y;
		Transform[1][1] = cosf(rotation) * scale.Y;
	}
	
	
	void Node::Rotate(float theta)
	{
		float rotation = GetRotation();
		
		SetRotation(rotation + theta);
	}
	
	
	Vec2 Node::GetScale()
	{
		Vec2 right = Vec2(Transform[0][0], Transform[0][1]);
		Vec2 up = Vec2(Transform[1][0], Transform[1][1]);
		
		Vec2 scale = Vec2(right.Magnitude(), up.Magnitude());
		
		if (p_parent)
			scale = scale + p_parent->GetScale();
		
		return scale;
	}
	
	
	void Node::SetScale(Vec2 scale)
	{
		Vec2 offset = Vec2();
		
		float rotation = GetRotation();
		
		if (p_parent)
			offset = p_parent->GetScale();
		
		Transform[0][0] = cosf(rotation) * scale.X;
		Transform[0][1] = -sinf(rotation) * scale.X;
		Transform[1][0] = sin(rotation) * scale.Y;
		Transform[1][1] = cosf(rotation) * scale.Y;
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