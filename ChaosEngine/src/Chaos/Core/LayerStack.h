#pragma once
#include "Chaos/Core/Core.h"
#include "Layer.h"

#include <vector>

namespace Chaos
{
	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_layers.end(); }
		std::vector<Layer*>::const_reverse_iterator  rbegin() { return m_layers.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator  rend() { return m_layers.rend(); }

	private: 
		std::vector<Layer*> m_layers; 
		unsigned int m_layerInsertIndex = 0;

	};
}