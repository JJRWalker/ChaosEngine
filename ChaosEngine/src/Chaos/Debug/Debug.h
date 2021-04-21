/* date = March 26th 2021 5:44 pm */

// static class with common debug funcs

#ifndef _DEBUG_H
#define _DEBUG_H

#include "Chaos/DataTypes/Vec2.h"
#include "Chaos/DataTypes/Vec4.h"

namespace Chaos
{
	class Debug
	{
		public:
		static void DrawLine(Vec2& start, Vec2& end, Vec4 colour = Vec4(1.0f, 1.0f, 1.0f, 1.0f), float weight = 0.02f);
	};
}

#endif //_DEBUG_H
