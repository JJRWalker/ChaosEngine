#include "chaospch.h"

#include "Debug.h"
#include "Chaos/Core/Application.h"
#include "Chaos/Renderer/Renderer.h"

const float DEBUG_RENDER_DEPTH = 1.0f;

namespace Chaos
{
	void Debug::DrawLine(Vec2& start, Vec2& end, Vec4 colour, float weight)
	{
		Renderer& renderer = Application::Get().GetRenderer();
		
		renderer.DrawLine(start, end, colour, weight, DEBUG_RENDER_DEPTH);
	}
}