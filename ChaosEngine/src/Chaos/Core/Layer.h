#pragma once

#include "Chaos/Core/Core.h"
#include "Chaos/Events/Event.h"

namespace Chaos
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer") 
			: m_debugName(name) {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnEvent(Event& event) {}
		virtual void OnImGuiUpdate() {}

		inline const std::string& GetName() const { return m_debugName; }

	protected:
		std::string m_debugName;
	};
}