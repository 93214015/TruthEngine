#pragma once


namespace TruthEngine::Core {

	class Layer;

	class LayerStack {

	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);

		std::vector<Layer*>::iterator Begin() { m_Layers.begin(); }
		std::vector<Layer*>::iterator End() { m_Layers.end(); }
		std::vector<Layer*>::reverse_iterator rBegin() { m_Layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rEnd() { m_Layers.rend(); }


		std::vector<Layer*>::const_iterator Begin() const { m_Layers.cbegin(); }
		std::vector<Layer*>::const_iterator End() const { m_Layers.cend(); }
		std::vector<Layer*>::const_reverse_iterator rBegin() const { m_Layers.crbegin(); }
		std::vector<Layer*>::const_reverse_iterator rEnd() const { m_Layers.crend(); }

	private:
		std::vector<Layer*> m_Layers;
		uint32_t m_LayerInsertIndex = 0;

	};

}