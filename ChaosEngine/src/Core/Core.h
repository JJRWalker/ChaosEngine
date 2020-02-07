#pragma once

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



#define  BIT(x) (1 << x)