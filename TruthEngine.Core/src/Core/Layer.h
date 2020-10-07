#pragma once
#include "Event/Event.h"

namespace TruthEngine::Core {

	class Layer
	{
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(double ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(const Event & event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}