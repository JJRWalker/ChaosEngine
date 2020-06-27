#pragma once
#include "chaospch.h"

namespace Chaos
{
	class Component
	{
	public:
		virtual void Start() const = 0;
		virtual void Update() const = 0;
		virtual void Destroy() const = 0;
	};
}