#pragma once
#include "Event/Event.h"

namespace TruthEngine::Core {

	class Layer
	{
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnUpdate(double deltaFrameTime) = 0;
		virtual void OnImGuiRender() = 0;
		virtual void OnEvent(TruthEngine::Core::Event & event) = 0;

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}