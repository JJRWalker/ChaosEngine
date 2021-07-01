#include "chaospch.h"

#include "Node.h"
#include "Math.h"
#include "Colliders.h"
#include "Chaos/Serialisation/Binary.h"

#include "Chaos/Core/Application.h"

// have to include all node types here in order to serialize them, not ideal but will have to do for the moment
#include <Chaos/Nodes/Camera.h>
#include <Chaos/Nodes/Sprite.h>
#include <Chaos/Nodes/Colliders.h>
#include <Chaos/Nodes/Animator.h>
#include <Chaos/Nodes/Lights.h>
#include <Chaos/Nodes/MeshRenderer.h>

namespace Chaos
{
	// Mainly used for serialization, will return a node of the type given via enum, node type must exist in ENodeType enum in Types.h
	Node* Node::Create(uint32_t type, bool child)
	{
		switch (type)
		{
			case NodeType::NODE: return new Node(child);
			case NodeType::CAMERA: return new Camera(child);
			case NodeType::SPRITE: return new Sprite(child);
			case NodeType::SUB_SPRITE: return new SubSprite(child);
			case NodeType::ANIMATOR: return new Animator(child);
			case NodeType::UI_SPRITE: return new UISprite(child);
			case NodeType::BOX_COLLIDER_2D: return new BoxCollider2D(child);
			case NodeType::CIRCLE_COLLIDER: return new CircleCollider(child);
			case NodeType::POINT_LIGHT_2D: return new PointLight2D(child);
			case NodeType::MESH_RENDERER: return new MeshRenderer(child);
		}
		
		// defined in user side GameApp, looks for types defined there
		return CreateUserDefinedNode(type, child);
	}
	
	
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
		Level* level = Level::Get();
		
		if (p_parent)
		{
			for (int i = SubID; i < p_parent->ChildCount; ++i)
			{
				Node* temp = level->Nodes[ID][i];
				level->Nodes[ID][i] = level->Nodes[ID][i + 1];
				level->Nodes[ID][i + 1] = temp;
				
				--level->Nodes[ID][i]->SubID;
			}
			
			level->Nodes[ID][p_parent->ChildCount] = nullptr;
			--p_parent->ChildCount;
		}
		else
		{
			while(level->Nodes[ID][1])
			{
				delete level->Nodes[ID][1];
			}
			
			for (int node = ID; node < level->NodeCount - 1; ++node)
			{
				Node** temp = level->Nodes[node];
				memcpy((void*)level->Nodes[node], (void*)level->Nodes[node + 1], sizeof(Node*) *+ MAX_CHILD_NODES);
				memcpy((void*)level->Nodes[node + 1], (void*)temp, sizeof(Node*) * MAX_CHILD_NODES);
				
				size_t nodeChildCount = level->Nodes[node][0]->ChildCount;
				
				for (size_t child = 0; child <= nodeChildCount; ++child)
				{
					--level->Nodes[node][child]->ID;
				}
			}
			
			size_t lastNodeIndex = level->NodeCount - 1;
			
			for (int child = 0; child < MAX_CHILD_NODES; ++child)
			{
				level->Nodes[lastNodeIndex][child] = nullptr;
			}
			
			--level->NodeCount;
		}
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
	
	
	void Node::OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle)
	{
	}
	
	
	void Node::OnDebug()
	{
	}
	
	
	void Node::SetEnabled(bool state)
	{
		Enabled = state;
	}
	
	
	bool Node::IsEnabled()
	{
		return Enabled;
	}
	
	
	// Really only sets the pending destruction flag to true. Kinda makes it less clear what's going on but reads better in code.
	void Node::Destroy()
	{
		PendingDestruction = true;
	}
	
	// recursively translates the local local matrix by the parent matrix to get global transform matrix
	float* Node::GetWorldTransform()
	{
		memcpy((void*)&m_globalTransform[0], (void*)&Transform[0], sizeof(float) * 16);
		
		if (p_parent)
		{
			float* parentTransform = p_parent->GetWorldTransform();
			
			m_globalTransform[0] *= parentTransform[0];
			m_globalTransform[1] *= parentTransform[1];
			m_globalTransform[2] *= parentTransform[2];
			
			m_globalTransform[4] *= parentTransform[4];
			m_globalTransform[5] *= parentTransform[5];
			m_globalTransform[6] *= parentTransform[6];
			
			m_globalTransform[8] *= parentTransform[8];
			m_globalTransform[9] *= parentTransform[9];
			m_globalTransform[10] *= parentTransform[10];
			
			m_globalTransform[12] += parentTransform[12];
			m_globalTransform[13] += parentTransform[13];
			m_globalTransform[14] += parentTransform[14];
		}
		
		return &m_globalTransform[0];
	}
	
	
	Vec2 Node::GetPosition()
	{
		Vec2 pos =  Vec2(Transform[12], Transform[13]);
		return pos;
	}
	
	
	Vec2 Node::GetWorldPosition()
	{
		Vec2 pos =  Vec2(Transform[12], Transform[13]);
		if (p_parent)
			pos = pos + p_parent->GetWorldPosition();
		return pos;
	}
	
	
	void Node::SetPosition(Vec2 position)
	{
		Transform[12] = position.X;
		Transform[13] = position.Y;
	}
	
	
	void Node::SetWorldPosition(Vec2 position)
	{
		Vec2 worldPosition = GetWorldPosition();
		if (p_parent)
			position = position + p_parent->GetWorldPosition();
		
		Transform[12] = position.X;
		Transform[13] = position.Y;
	}
	
	
	Vec3 Node::GetPosition3D()
	{
		Vec3 pos = Vec3(Transform[12], Transform[13], Transform[14]);
		return pos;
	}
	
	
	Vec3 Node::GetWorldPosition3D()
	{
		Vec3 pos = Vec3(Transform[12], Transform[13], Transform[14]);
		if (p_parent)
			pos = pos + p_parent->GetWorldPosition3D();
		return pos;
	}
	
	
	void Node::SetPosition(Vec3 position)
	{
		Transform[12] = position.X;
		Transform[13] = position.Y;
		Transform[14] = position.Z;
	}
	
	
	void Node::SetWorldPosition(Vec3 position)
	{
		if (p_parent)
			position = position + p_parent->GetWorldPosition3D();
		Transform[12] = position.X;
		Transform[13] = position.Y;
		Transform[14] = position.Z;
	}
	
	
	float Node::GetDepth()
	{
		return Transform[14];
	}
	
	
	float Node::GetWorldDepth()
	{
		float depth = Transform[14];
		if (p_parent)
			depth += GetWorldDepth();
		return Transform[14];
	}
	
	
	void Node::SetDepth(float depth)
	{
		Transform[14] = depth;
	}
	
	
	void Node::SetWorldDepth(float depth)
	{
		if (p_parent)
			depth += p_parent->GetWorldDepth();
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
	
	
	float Node::GetWorldRotation()
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
		
		rotation += offset;
		//rotation = rotation - (int)(rotation / PI) * PI;
		
		Transform[0] = cosf(rotation) * scale.X;
		Transform[1] = -sinf(rotation) * scale.X;
		Transform[4] = sinf(rotation) * scale.Y;
		Transform[5] = cosf(rotation) * scale.Y;
	}
	
	
	void Node::SetWorldRotation(float rotation)
	{
		Vec2 scale = GetScale();
		
		float offset = 0;
		
		if (p_parent)
			offset = p_parent->GetRotation();
		
		rotation += offset;
		//rotation = rotation - (int)(rotation / PI) * PI;
		
		Transform[0] = cosf(rotation) * scale.X;
		Transform[1] = -sinf(rotation) * scale.X;
		Transform[4] = sinf(rotation) * scale.Y;
		Transform[5] = cosf(rotation) * scale.Y;
	}
	
	
	void Node::Rotate(float rotation)
	{
		float currentRotation = GetRotation();
		
		SetRotation(currentRotation + rotation);
	}
	
	
	Vec2 Node::GetScale()
	{
		Vec2 right = Vec2(Transform[0], Transform[1]);
		Vec2 up = Vec2(Transform[4], Transform[5]);
		
		int rightMod = right.X + right.Y < 0 ? -1 : 1;
		int upMod = up.X + up.Y < 0 ? -1 : 1;
		
		Vec2 scale = Vec2(right.Magnitude(), up.Magnitude());
		
		return scale;
	}
	
	
	Vec2 Node::GetWorldScale()
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
		
		Transform[0] = cosf(rotation) * scale.X;
		Transform[1] = -sinf(rotation) * scale.X;
		Transform[4] = sinf(rotation) * scale.Y;
		Transform[5] = cosf(rotation) * scale.Y;
	}
	
	
	void Node::SetWorldScale(Vec2 scale)
	{
		Vec2 offset = Vec2();
		
		float rotation = GetRotation();
		
		if (p_parent)
			offset = p_parent->GetScale();
		
		Transform[0] = cosf(rotation) * scale.X;
		Transform[1] = -sinf(rotation) * scale.X;
		Transform[4] = sinf(rotation) * scale.Y;
		Transform[5] = cosf(rotation) * scale.Y;
	}
	
	
	Binary Node::SaveToBinary()
	{
		size_t finalDataSize = 0;
		
		Binary version((void*)&m_nodeVersion, sizeof(m_nodeVersion));
		finalDataSize += sizeof(m_nodeVersion);
		
		Binary type((void*)&Type, sizeof(Type));
		finalDataSize += sizeof(Type);
		
		size_t nameLen = Name.size() + 1;
		Binary nameSize((void*)&nameLen, sizeof(size_t));
		finalDataSize += sizeof(size_t);
		
		Binary name((void*)&Name[0], nameLen);
		finalDataSize += nameLen; // char is 1 byte so we can just add the length
		
		Binary id((void*)&ID, sizeof(ID));
		Binary subId((void*)&SubID, sizeof(SubID));
		finalDataSize += sizeof(ID);
		finalDataSize += sizeof(SubID);
		
		Binary childCount((void*)&ChildCount, sizeof(ChildCount));
		finalDataSize += sizeof(ChildCount);
		
		Binary transform((void*)&Transform, sizeof(Transform));
		Binary globalTransform((void*)&m_globalTransform, sizeof(m_globalTransform));
		finalDataSize += sizeof(Transform);
		finalDataSize += sizeof(m_globalTransform);
		
		Binary enabled((void*)&Enabled, sizeof(Enabled));
		finalDataSize += sizeof(Enabled);
		
		// need to copy this data as once the Binary object leaves scope it'll take it with it.
		Binary data(finalDataSize);
		data.Write(version.Data, version.Capacity());
		data.Write(type.Data, type.Capacity());
		data.Write(nameSize.Data, nameSize.Capacity());
		data.Write(name.Data, name.Capacity());
		data.Write(id.Data, id.Capacity());
		data.Write(subId.Data, subId.Capacity());
		data.Write(childCount.Data, childCount.Capacity());
		data.Write(transform.Data, transform.Capacity());
		data.Write(globalTransform.Data, globalTransform.Capacity());
		data.Write(enabled.Data, enabled.Capacity());
		
		return data;
	}
	
	
	void Node::LoadFromBinary(char* data)
	{
		memcpy((void*)&m_nodeVersion, (void*)data, sizeof(uint32_t));
		size_t location = sizeof(uint32_t);
		
		switch (m_nodeVersion)
		{
			case NODE_VERSION_0:
			{
				memcpy((void*)&Type, (void*)&data[location], sizeof(size_t));
				location += sizeof(Type);
				
				size_t namelen;
				memcpy((void*)&namelen, (void*)&data[location], sizeof(size_t));
				location += sizeof(size_t);
				
				memcpy((void*)&Name[0], (void*)&data[location], namelen);
				location += namelen;
				
				memcpy((void*)&ID, (void*)&data[location], sizeof(ID));
				location += sizeof(ID);
				
				memcpy((void*)&SubID, (void*)&data[location], sizeof(SubID));
				location += sizeof(SubID);
				
				memcpy((void*)&ChildCount, (void*)&data[location], sizeof(ChildCount));
				location += sizeof(ChildCount);
				
				memcpy((void*)&Transform, (void*)&data[location], sizeof(Transform));
				location += sizeof(Transform);
				
				memcpy((void*)&m_globalTransform, (void*)&data[location], sizeof(m_globalTransform));
				location += sizeof(m_globalTransform);
				
				memcpy((void*)&Enabled, (void*)&data[location], sizeof(Enabled));
				
				DebugEnabled = false;
				PendingDestruction = false;
			} break;
			
			default: 
			{
				LOGCORE_ERROR("NODE: LoadFromBinary: Loading from version number {0} not supported in this engine version!", m_nodeVersion);
			} break;
		}
	}
	
	
	size_t Node::GetSize()
	{
		return sizeof(*this);
	}
	
	
	const char* Node::GetType()
	{
		return typeid(*this).name();
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