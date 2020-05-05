#pragma once

#include <memory>

#ifdef CHAOS_PLATFORM_WINDOWS

#else
#error Chaos only supports windows!
#endif

#ifdef CHAOS_DEBUG
	#define CHAOS_ENABLE_ASSERTS
#endif

#ifdef CHAOS_ENABLE_ASSERTS
	#define  ASSERT(x, ...){if (!(x)){LOGERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define  COREASSERT(x, ...){if (!(x)){LOGCORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define ASSERT(x,...)
	#define COREASSERT(x,...)
#endif

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define  BIT(x) (1 << x)

namespace Chaos
{
	//Method that takes in any type and returns a shared pointer for that type.
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}