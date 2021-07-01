/* date = January 30th 2021 11:02 am */

#ifndef _NODE_H
#define _NODE_H

#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec3.h"
#include "Chaos/Core/Core.h"
#include "Chaos/Core/Level.h"
#include "Chaos/Core/Types.h"
#include "Chaos/Serialisation/Binary.h"

#include <string>

namespace Chaos
{
	const uint32_t NODE_VERSION_0 = 0;
	
	class Collider;
	class Texture;
	class Node 
	{
		friend class Level;
		
		public:
		Node(bool child = false);
		virtual ~Node();
		
		static Node* Node::Create(uint32_t type, bool child); // defined in Game/TypeRegistry.h
		
		virtual void OnStart();                  // called on start of level
		virtual void OnUpdate(float delta);      // called every frame
		virtual void OnFixedUpdate(float delta); // called on specified fixed step
		virtual void SetEnabled(bool state);
		virtual bool IsEnabled();
		void Destroy();
		
		virtual void OnDebug();
		virtual void OnShowEditorDetails(Texture* editorTexture, void* editorImageHandle); // NOTE: void* because I'm not sure we want to be tied to imgui on this side of things
		
		// returns a 16 float array of all transforms added up through heirarchy
		virtual float* GetWorldTransform();
		virtual Vec2 GetPosition();
		virtual Vec2 GetWorldPosition();
		virtual void SetPosition(Vec2 position);
		virtual void SetWorldPosition(Vec2 position);
		virtual Vec3 GetPosition3D();
		virtual Vec3 GetWorldPosition3D();
		virtual void SetPosition(Vec3 position);
		virtual void SetWorldPosition(Vec3 position);
		virtual float GetDepth(); // refers to render depth / order, basically the z comp of pos
		virtual float GetWorldDepth();
		virtual void SetDepth(float depth);
		virtual void SetWorldDepth(float depth);
		virtual void Translate(Vec2 translation);
		// only one rotation value we're concerned about in 2D, z
		virtual float GetRotation();
		virtual float GetWorldRotation();
		virtual void SetRotation(float rotation);
		virtual void SetWorldRotation(float rotation);
		virtual void Rotate(float rotation);
		virtual Vec2 GetScale();
		virtual Vec2 GetWorldScale();
		virtual void SetScale(Vec2 scale);
		virtual void SetWorldScale(Vec2 scale);
		
		virtual Node* GetParent() {return p_parent;}
		
		// for seralization
		virtual Binary SaveToBinary();
		virtual void LoadFromBinary(char* data);
		size_t GetSize();
		const char* GetType();
		
		// collision funcs called when a child collider node triggers any conditions
		virtual void ColliderStay(Collider* self, Collider* other);
		virtual void ColliderEnter(Collider* self, Collider* other);
		virtual void ColliderExit(Collider* self, Collider* other);
		virtual void TriggerStay(Collider* self, Collider* other);
		virtual void TriggerEnter(Collider* self, Collider* other);
		virtual void TriggerExit(Collider* self, Collider* other);
		
		template <typename T, typename... Args> T* AddChild(Args... args)
		{
			if(std::is_base_of<Node, T>::value)
			{
				T* node = new T(args..., true);
				node->ID = ID;
				node->SubID = (uint32_t)ChildCount + 1;
				Level::Get()->Nodes[ID][ChildCount + 1] = node;
				node->p_parent = this;
				ChildCount++; // need to increment after else this causes issues trying to access a child that doesn't exist on fixed update
				return node;
			}
			else
			{
				LOGCORE_ERROR("NODE: {0}: Tried to add a node that was not derived from the Node class, returning nullptr!", Name);
			}
			return nullptr;
		}
		
		
		template <typename T> void AddChild(T* child)
		{
			if (std::is_base_of<Node, T>::value)
			{
				ChildCount++;
				child->ID = ID;
				child->SubID = ChildCount;
				Level::Get()->Nodes[ID][ChildCount] = child;
				child->p_parent = this;
			}
		}
		
		
		template <typename T> T* GetChild()
		{
			Level* level = Level::Get();
			for (int i = 1; i <= ChildCount; ++i)
			{
				T* node = dynamic_cast<T*>(level->Nodes[ID][i]);
				if (node)
				{
					return node;
				}
			}
			LOGCORE_ERROR("NODE: {0}: Failed to get component on node returning nullptr!", Name);
			return nullptr;
		}
		
		
		template <typename T> T* GetChild(int index)
		{
			if (index >= ChildCount)
			{
				LOGCORE_ERROR("NODE: {0}: Get child index out of range, returning nullptr!", Name);
				return nullptr;
			}
			return Level::Get()->Nodes[ID][index];
		}
		
		
		template <typename T> T* GetChildren(size_t& size)
		{
			T* returnedNodes[MAX_CHILD_NODES];
			memset(returnedNodes, 0, MAX_CHILD_NODES * sizeof(T*));
			size_t insert = 0;
			Level* level = Level::Get();
			for (int i = 1; i <= ChildCount; ++i)
			{
				T* node = dynamic_cast<T*>(level->Nodes[ID][i]);
				if (node)
				{
					returnedNodes[insert] = node;
					++insert;
				}
			}
			
			size = insert;
			
			return returnedNodes[0];
		}
		
		
		template <typename T> bool HasChild()
		{
			Level* level = Level::Get();
			for (int i = 1; i <= ChildCount; ++i)
			{
				T* node = dynamic_cast<T*>(level->Nodes[ID][i]);
				if (node)
				{
					return true;
				}
			}
			return false;
		}
		
		
		public:
		std::string Name = "Node";  // available for when there is simply no other way to get a node
		uint32_t Type = NodeType::NODE;
		uint32_t ID = 0;  // designates it's index on the level array
		uint32_t SubID = 0; // if it's a child this sub ID will be it's index in the 2nd dimension of the level array
		size_t ChildCount = 0;
		bool DebugEnabled = false;
		
		bool PendingDestruction = false;
		
		float Transform[16] = 
		{1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1};
		
		private:
		// Private as it needs to be recalculated each time we want to get it.
		float m_globalTransform[16] = 
		{1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1}; 
		
		protected: 
		Node* p_parent = nullptr;
		
		bool Enabled = true;  // same as can tick, disabled nodes still exist, just don't update in update
		
		uint32_t m_nodeVersion = 0;
	};
	
	extern Node* CreateUserDefinedNode(uint32_t type, bool child);
}

#endif //_NODE_H
