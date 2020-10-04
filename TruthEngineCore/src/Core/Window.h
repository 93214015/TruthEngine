#pragma once
#include "TruthEngine.Core.h"
#include "Event/Event.h"

namespace TruthEngine::Core {
	
	class Window {

	public:
		Window(const char* title, uint16_t width, uint16_t height)  : m_Title(title), m_Width(width), m_Height(height)
		{
		}


		using EventCallBack = std::function<void(const Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual void SetEventCallBack(const EventCallBack& fn) = 0;

		uint16_t GetWidth() { return m_Width; }
		uint16_t GetHeight() { return m_Height; }

	protected:
		std::string m_Title;
		uint16_t m_Width;
		uint16_t m_Height;

	};

	std::unique_ptr<Window> CreateWindow(const char* title, uint16_t width, uint16_t height);
}
