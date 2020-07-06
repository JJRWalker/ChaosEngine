#pragma once
#include "chaospch.h"

namespace Chaos
{
	class IComponent
	{
	public:
		virtual void Start() = 0;
		virtual void Update() = 0;
		virtual void Destroy() = 0;
	};
}