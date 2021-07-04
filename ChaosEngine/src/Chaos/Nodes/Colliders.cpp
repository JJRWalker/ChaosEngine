#include "chaospch.h"

#include "colliders.h"
#include "Chaos/Maths/Collisions.h"
#include "Chaos/DataTypes/QuadTree.h"

// debug
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Debug/ImGuiLayer.h"

const float DEBUG_LINE_WEIGHT = 0.025f;
const float DEBUG_RENDER_ORDER = 1.0f;

namespace Chaos
{
	Collider::Collider() : ColliderType(EColliderType::NONE)
	{
		Name = "Collider";
		
		memset((void*)Overlaps, 0, sizeof(Overlaps));
	}
	
	
	Collider::~Collider()
	{
		for (int i = 0; i < OverlapsSize; ++i)
		{
			Overlaps[i]->RemoveOverlap(this);
		}
	}
	
	
	void Collider::OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle)
	{
		int objectMask = 0;
		int collisionMask = 0;
		
		const char* updateTypeOptions[] = { "Per Frame", "Fixed Step" };
		int selectedUpdateType = (int)UpdateType;
		
		if (ImGui::DragInt("Object Mask", &(int)ObjectMask))
		{
			
		}
		
		if (ImGui::DragInt("Collsion Mask", &(int)CollisionMask))
		{
		}
		
		
		if (ImGui::Combo("Update Type", &selectedUpdateType, updateTypeOptions, IM_ARRAYSIZE(updateTypeOptions)))
		{
			UpdateType = (EPhysicsUpdateType)selectedUpdateType;
		}
		
		ImGui::Checkbox("Trigger", &Trigger);
		
		ImGui::Separator();
	}
	
	
	void Collider::CheckCollisionExit()
	{
		// colliders leaving
		for (size_t i = 0; i < OverlapsSize; ++i)
		{
			// iterate over overlaps and compare to see if something has left
			bool foundHitNodeInOverlaps = false;
			for (size_t j = 0; j < m_hitNodesSize; ++j)
			{
				if (Overlaps[i] == m_hitNodes[j])
				{
					foundHitNodeInOverlaps = true;
					break;
				}
			}
			
			if (!foundHitNodeInOverlaps)
			{
				RemoveOverlap(Overlaps[i]);
				--i;
			}
		}
	}
	
	
	// inserts single node into overlaps, calls enter and stay methods for nodes.
	void Collider::InsertOverlap(Collider* collider)
	{
		Level* level = Level::Get();
		Node* root = level->Nodes[ID];
		
		while (root->Parent)
		{
			root = root->Parent;
		}
		
		bool alreadyInOvelaps = false;
		for (size_t j = 0; j < OverlapsSize; ++j)
		{
			if (collider == Overlaps[j])
			{
				alreadyInOvelaps = true;
				
				//size_t childCount;
				//Node** allChildren = root->GetAllChildren(childCount);
				
				if (collider->Trigger)
					root->TriggerStay(this, collider);
				else
					root->ColliderStay(this, collider);
				
				//for (size_t j = 0; j < childCount; ++j)
				//{
				//if (collider->Trigger)
				//allChildren[j]->TriggerStay(this, collider);
				//else
				//allChildren[j]->ColliderStay(this, collider);
				//}
				
				//free(allChildren);
			}
		}
		if (!alreadyInOvelaps)
		{
			size_t childCount;
			Node** allChildren = root->GetAllChildren(childCount, true);
			
			if (collider->Trigger)
				root->TriggerEnter(this, collider);
			else
				root->ColliderEnter(this, collider);
			
			for (size_t j = 0; j < childCount; ++j)
			{
				if (collider->Trigger)
					allChildren[j]->TriggerEnter(this, collider);
				else
					allChildren[j]->ColliderEnter(this, collider);
			}
			Overlaps[OverlapsSize] = collider;
			++OverlapsSize;
			
			free(allChildren);
		}
	}
	
	
	
	// inserts nodes into overlaps, calls enter and stay methods for nodes.
	void Collider::InsertOverlaps(Collider** colliders, size_t size)
	{
		PROFILED_FUNC();
		for (int i = 0; i < size; ++i)
		{
			InsertOverlap(colliders[i]);
		}
	}
	
	
	void Chaos::Collider::RemoveOverlap(Collider* collider)
	{
		Level* level = Level::Get();
		Node* root = level->Nodes[ID];
		
		while (root->Parent)
		{
			root = root->Parent;
		}
		
		size_t childCount;
		Node** allChildren = root->GetAllChildren(childCount, true);
		
		if (collider->Trigger)
			root->TriggerExit(this, collider);
		else
			root->ColliderExit(this, collider);
		
		for (size_t j = 0; j < childCount; ++j)
		{
			if (collider->Trigger)
				allChildren[j]->TriggerExit(this, collider);
			else
				allChildren[j]->ColliderExit(this, collider);
		}
		
		free(allChildren);
		
		
		for (size_t i = 0; i < OverlapsSize; ++i)
		{
			if (Overlaps[i] == collider)
			{
				// move to be removed to the end of the array
				for (size_t j = i; j < OverlapsSize - 1; ++j)
				{
					Collider* swapTemp = Overlaps[j];
					Overlaps[j] = Overlaps[j + 1];
					Overlaps[j + 1] = swapTemp;
				}
				
				Overlaps[OverlapsSize - 1] = nullptr;
				--OverlapsSize;
				return;
			}
		}
	}
	
	
	bool Collider::OverlapsWith(Collider* other)
	{
		for (int i = 0; i < OverlapsSize; ++i)
		{
			if (Overlaps[i] == other)
			{
				return true;
			}
		}
		
		return false;
	}
	
	
	void Collider::ClearOverlaps()
	{
		memset(Overlaps, 0, sizeof(Overlaps));
		
		OverlapsSize = 0;
	}
	
	
	void Chaos::Collider::InsertHitNode(Collider* collider)
	{
		m_hitNodes[m_hitNodesSize] = collider;
		++m_hitNodesSize;
	}
	
	
	BoxCollider2D::BoxCollider2D()
	{
		Name = "BoxCollider2D";
		ColliderType = EColliderType::BOX2D;
		Type = NodeType::BOX_COLLIDER_2D;
	}
	
	
	void BoxCollider2D::OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle)
	{
		Collider::OnShowEditorDetails(editorTexture, editorImageHandle);
		ImGui::DragFloat2("Bounds", &Bounds.X, 0.01f);
	}
	
	
	void BoxCollider2D::OnDebug()
	{
		Renderer& renderer = Application::Get().GetRenderer();
		
		Vec2 pos = GetWorldPosition();
		
		Vec2 topLeft = Vec2(pos.X - Bounds.X, pos.Y + Bounds.Y);
		Vec2 topRight = Vec2(pos.X + Bounds.X , pos.Y + Bounds.Y );
		Vec2 bottomRight = Vec2(pos.X + Bounds.X , pos.Y - Bounds.Y );
		Vec2 bottomLeft = Vec2(pos.X - Bounds.X , pos.Y - Bounds.Y );
		
		renderer.DrawLine(topLeft, topRight, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, DEBUG_RENDER_ORDER);
		renderer.DrawLine(topRight, bottomRight, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, DEBUG_RENDER_ORDER);
		renderer.DrawLine(bottomRight, bottomLeft, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, DEBUG_RENDER_ORDER);
		renderer.DrawLine(bottomLeft, topLeft, Vec4(0.0f, 1.0f, 0.0f, 0.8f), DEBUG_LINE_WEIGHT, DEBUG_RENDER_ORDER);
	}
	
	
	bool BoxCollider2D::CollideWith(Collider* other)
	{
		LOGCORE_WARN("BoxCollider::CollideWith not implemented!");
		return false;
	}
	
	
	void BoxCollider2D::CheckCollisions(QuadTree* tree)
	{
		// clear hit nodes (should only contain nodes from last check this object did
		memset(m_hitNodes, 0, sizeof(m_hitNodes));
		m_hitNodesSize = 0;
		
		Collider** nodesInRange = (Collider**)malloc(MAX_COLLIDER_OVERLAPS * sizeof(Collider*));
		size_t nodesInRangesize = 0;
		tree->QueryRange(GetWorldPosition(), Bounds, nodesInRange, nodesInRangesize);
		
		for (size_t i = 0; i < nodesInRangesize; ++i)
		{
			if (!HASBIT(CollisionMask, nodesInRange[i]->ObjectMask) || nodesInRange[i] == this || nodesInRange[i]->ID == ID)
				continue;
			
			m_hitNodes[m_hitNodesSize] = nodesInRange[i];
			++m_hitNodesSize;
		}
		
		InsertOverlaps(m_hitNodes, m_hitNodesSize);
		
		for (size_t i = 0; i < m_hitNodesSize; ++i)
		{
			m_hitNodes[i]->InsertOverlap(this);
			m_hitNodes[i]->InsertHitNode(this);
		}
		
		free(nodesInRange);
	}
	
	
	CircleCollider::CircleCollider()
	{
		Name = "CircleCollider";
		ColliderType = EColliderType::CIRCLE;
		Type = NodeType::CIRCLE_COLLIDER;
	}
	
	
	void CircleCollider::OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle)
	{
		Collider::OnShowEditorDetails(editorTexture, editorImageHandle);
		ImGui::DragFloat("Radius", &Radius, 0.01f);
	}
	
	
	void CircleCollider::OnDebug()
	{
		// not really optimal, but these are only for debugging so it shouldn't matter too much
		Renderer& renderer = Application::Get().GetRenderer();
		
		Vec2 pos = GetWorldPosition();
		int steps = 60;
		
		float stepIncrement = (2 * PI) / steps;
		
		for (int i = 0; i < steps; ++i)
		{
			Vec2 start = Vec2(pos.X + Radius * cos(i * stepIncrement), pos.Y + Radius * sin(i * stepIncrement));
			Vec2 end = Vec2(pos.X + Radius * cos((i + 1) * stepIncrement), pos.Y + Radius * sin((i + 1) * stepIncrement));
			
			renderer.DrawLine(start, end, Vec4(0.3f, 1.0f, 0.3f, 0.8f), DEBUG_LINE_WEIGHT, DEBUG_RENDER_ORDER);
		}
	}
	
	
	bool CircleCollider::CollideWith(Collider* other)
	{
		LOGCORE_WARN("CircleCollider::CollideWith not implemented!");
		return false;
	}
	
	
	void CircleCollider::CheckCollisions(QuadTree* tree)
	{
		// clear hit nodes(should only contain nodes hit this collision check)
		memset(m_hitNodes, 0, sizeof(m_hitNodes));
		m_hitNodesSize = 0;
		
		Collider** nodesInRange = (Collider**)malloc(MAX_COLLIDER_OVERLAPS * sizeof(Collider*));
		size_t nodesInRangesize = 0;
		tree->QueryRadius(GetWorldPosition(), Radius, nodesInRange, nodesInRangesize);
		
		for (size_t i = 0; i < nodesInRangesize; ++i)
		{
			if (!HASBIT(CollisionMask, nodesInRange[i]->ObjectMask) || nodesInRange[i] == this || nodesInRange[i]->ID == ID)
				continue;
			
			m_hitNodes[m_hitNodesSize] = nodesInRange[i];
			++m_hitNodesSize;
		}
		
		InsertOverlaps(m_hitNodes, m_hitNodesSize);
		
		for (size_t i = 0; i < m_hitNodesSize; ++i)
		{
			m_hitNodes[i]->InsertOverlap(this);
			m_hitNodes[i]->InsertHitNode(this);
		}
		
		free(nodesInRange);
	}
}