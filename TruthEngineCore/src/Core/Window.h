#pragma once
#include "TruthEngine.Core.h"
#include "Event/Event.h"

namespace TruthEngine::Core {

	class Window {

	public:

		using EventCallBack = std::function<void(const Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual void SetEventCallBack(const EventCallBack& fn) = 0;

		uint16_t GetWidth() { return m_Width; }
		uint16_t GetHeight() { return m_Height; }

		virtual std::unique_ptr<Window> Create() = 0;
	protected:


	protected:
		std::string m_Title;
		uint16_t m_Width;
		uint16_t m_Height;

	};

}
