#include "chaospch.h"
#include "RendererAPI.h"

namespace Chaos
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;	//Currently hard set to vulkan, should be changed later dependant on platform or settings
}