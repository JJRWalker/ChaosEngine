/* date = June 30th 2021 7:11 pm */

#ifndef _TYPE_REGISTRY_H
#define _TYPE_REGISTRY_H

#include "Chaos/Nodes/Node.h"

namespace NodeType
{
	// define custom node types here as uint32_t and use in the switch statement in CreateUserDefinedNode
}


namespace Chaos
{
	Node* CreateUserDefinedNode(uint32_t type, bool child);
}

#endif //_TYPE_REGISTRY_H
