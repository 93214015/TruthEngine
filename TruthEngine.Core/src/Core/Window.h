#pragma once
#include "Event/Event.h"

namespace TruthEngine::Core {

	class Window {

	public:
		using EventCallBack = std::function<void(Event&)>;

		Window(const char* title, uint16_t width, uint16_t height) : m_Data{ title, width , height, nullptr }
		{
		}

		struct WindowData {
			std::string Title;
			uint16_t Width;
			uint16_t Height;
			EventCallBack CallBack;
		};


		virtual ~Window() = default;

		virtual void Show() = 0;

		virtual void OnUpdate() = 0;

		virtual void* GetNativeWindowHandle() = 0;

		virtual void SetEventCallBack(const EventCallBack& fn) { m_Data.CallBack = fn; };

		virtual void ToggleFullScreenMode() = 0;

		uint16_t GetWidth() { return m_Data.Width; }
		uint16_t GetHeight() { return m_Data.Height; }

	protected:


	protected:

		WindowData m_Data;

	};

	std::unique_ptr<Window> TECreateWindow(const char* title, uint32_t width, uint32_t height);
}
