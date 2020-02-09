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

		std::vector<Layer*>::iterator begin() { return mLayers.begin(); }
		std::vector<Layer*>::iterator end() { return mLayers.end(); }
		std::vector<Layer*>::const_reverse_iterator  rbegin() { return mLayers.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator  rend() { return mLayers.rend(); }

	private: 
		std::vector<Layer*> mLayers; 
		unsigned int mLayerInsertIndex = 0;

	};
}