#pragma once
#include "Event/Event.h"

namespace TruthEngine {

	class Layer
	{
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnUpdate(double deltaFrameTime) {};
		virtual void OnImGuiRender() {};
		virtual void OnEvent(TruthEngine::Event& event) {};

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}