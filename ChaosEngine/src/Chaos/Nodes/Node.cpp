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
	Node* Node::Create(uint32_t type)
	{
		switch (type)
		{
			case NodeType::NODE: return new Node();
			case NodeType::CAMERA: return new Camera();
			case NodeType::SPRITE: return new Sprite();
			case NodeType::SUB_SPRITE: return new SubSprite();
			case NodeType::ANIMATOR: return new Animator();
			case NodeType::UI_SPRITE: return new UISprite();
			case NodeType::BOX_COLLIDER_2D: return new BoxCollider2D();
			case NodeType::CIRCLE_COLLIDER: return new CircleCollider();
			case NodeType::POINT_LIGHT_2D: return new PointLight();
			case NodeType::MESH_RENDERER: return new MeshRenderer();
		}
		
		// defined in user side GameApp, looks for types defined there
		return CreateUserDefinedNode(type);
	}
	
	
	Node::Node()
	{
		ID = Level::Get()->NodeCount;
		Level::Get()->Nodes[ID] = this;
		
		Level::Get()->NodeCount++;
	}
	
	
	Node::~Node()
	{
		Level* level = Level::Get();
		
		if (Parent)
		{
			Parent->Children.Remove(this);
		}
		
		while (Children[0])
		{
			delete Children[0];
		}
		
		for (int i = ID; i < level->NodeCount; ++i)
		{
			level->Nodes[i] = level->Nodes[i + 1];
			if (level->Nodes[i])
				--level->Nodes[i]->ID;
		}
		
		level->Nodes[level->NodeCount - 1] = nullptr;
		--level->NodeCount;
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
	
	
	// sets destroy flag for it will destroy children on delete
	void Node::Destroy()
	{
		PendingDestruction = true;
	}
	
	// recursively translates the local local matrix by the parent matrix to get global transform matrix
	float* Node::GetWorldTransform()
	{
		memcpy((void*)&m_globalTransform[0], (void*)&Transform[0], sizeof(float) * 16);
		
		if (Parent)
		{
			float* parentTransform = Parent->GetWorldTransform();
			
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
		if (Parent)
			pos = pos + Parent->GetWorldPosition();
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
		if (Parent)
			position = position + Parent->GetWorldPosition();
		
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
		if (Parent)
			pos = pos + Parent->GetWorldPosition3D();
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
		if (Parent)
			position = position + Parent->GetWorldPosition3D();
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
		if (Parent)
			depth += GetWorldDepth();
		return Transform[14];
	}
	
	
	void Node::SetDepth(float depth)
	{
		Transform[14] = depth;
	}
	
	
	void Node::SetWorldDepth(float depth)
	{
		if (Parent)
			depth += Parent->GetWorldDepth();
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
		if (Parent)
			theta += Parent->GetRotation();
		return theta;
	}
	
	
	float Node::GetWorldRotation()
	{
		float theta = atan2(Transform[4], Transform[0]);
		if (Parent)
			theta += Parent->GetRotation();
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
		
		if (Parent)
			offset = Parent->GetRotation();
		
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
		
		if (Parent)
			scale = scale * Parent->GetScale();
		
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
		
		if (Parent)
			offset = Parent->GetScale();
		
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
		
		size_t nameLen = strlen(Name.c_str()) + 1;
		Binary nameSize((void*)&nameLen, sizeof(size_t));
		finalDataSize += sizeof(size_t);
		
		Binary name((void*)&Name[0], nameLen);
		finalDataSize += nameLen; // char is 1 byte so we can just add the length
		
		Binary id((void*)&ID, sizeof(ID));
		finalDataSize += sizeof(ID);
		
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
		data.Write(transform.Data, transform.Capacity());
		data.Write(globalTransform.Data, globalTransform.Capacity());
		data.Write(enabled.Data, enabled.Capacity());
		
		return data;
	}
	
	
	size_t Node::LoadFromBinary(char* data)
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
				
				Name.resize(namelen - 1);
				memcpy((void*)&Name[0], (void*)&data[location], namelen);
				location += namelen;
				
				memcpy((void*)&ID, (void*)&data[location], sizeof(ID));
				location += sizeof(ID);
				
				memcpy((void*)&Transform, (void*)&data[location], sizeof(Transform));
				location += sizeof(Transform);
				
				memcpy((void*)&m_globalTransform, (void*)&data[location], sizeof(m_globalTransform));
				location += sizeof(m_globalTransform);
				
				bool enabled;
				
				memcpy((void*)&enabled, (void*)&data[location], sizeof(enabled));
				location += sizeof(enabled);
				
				// need to set enabled here rather than just loading the value into Enabled
				// certain nodes override this, not calling set enabled causes issues on load
				SetEnabled(enabled);
				
				DebugEnabled = false;
				PendingDestruction = false;
				
				return location;
			} break;
			
			default: 
			{
				LOGCORE_ERROR("NODE: LoadFromBinary: Loading from version number {0} not supported in this engine version!", m_nodeVersion);
			} break;
		}
		
		return 0;
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
	
	
	// NOTE: remember to free() when done with the data
	Node** Node::GetAllChildren(size_t& size, bool recursive)
	{
		
		PROFILED_FUNC();
		size = 0;
		Node** returnedNodes = (Node**)malloc(sizeof(Node*) * MAX_NODES);
		
		
		if (recursive)
		{
			GetAllChildrenRecursively(size, returnedNodes);
			return returnedNodes;
		}
		
		for (int child = 0; child < Children.Size(); ++child)
		{
			returnedNodes[size] = Children[child];
			++size;
		}
		
		return returnedNodes;
	}
	
	
	void Node::GetAllChildrenRecursively(size_t& size, Node** nodesOut)
	{
		for (int child = 0; child < Children.Size(); ++child)
		{
			nodesOut[size] = Children[child];
			++size;
			Children[child]->GetAllChildrenRecursively(size, nodesOut);
		}
	}
}