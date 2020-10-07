#include "pch.h"
#include "LayerStack.h"
#include "Core/Layer.h"

namespace TruthEngine::Core {



	LayerStack::~LayerStack()
	{
		for (auto layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.cbegin() + m_LayerInsertIndex, layer);
		layer->OnAttach();
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* layer)
	{
		m_Layers.emplace_back(layer);
		layer->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.cbegin(), m_Layers.cbegin() + m_LayerInsertIndex, layer);
		if (it != m_Layers.cbegin() + m_LayerInsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* layer)
	{
		auto it = std::find(m_Layers.cbegin() + m_LayerInsertIndex, m_Layers.cend(), layer);
		if (it != m_Layers.cend())
		{
			layer->OnDetach();
			m_Layers.erase(it);
		}
	}

}