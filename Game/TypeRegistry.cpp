#include "chaospch.h"

#include "TypeRegistry.h"

// have to include all Game node types here in order to serialize them, not ideal but will have to do for the moment

namespace Chaos
{
	// Mainly used for serialization, called from static Node::Create() when loading in a new level. 
	Node* CreateUserDefinedNode(uint32_t type, bool child)
	{
		switch (type)
		{
			// add cases to return custom types here...
		}
		
		LOGCORE_ERROR("NODE: Create: NodeType {0} not found in node registry, could you have forgotten to add it to TypeRegistry.cpp?", type);
		LOGCORE_INFO("NODE: Create: Spawning default node instead");
		
		return new Node(child);
	}
	
}