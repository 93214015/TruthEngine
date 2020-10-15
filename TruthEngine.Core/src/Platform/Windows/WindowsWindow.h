#pragma once
#include "Core/Window.h"

namespace TruthEngine::Platforms::Windows {

	class WindowsWindow : public TruthEngine::Core::Window
	{
	public:
		WindowsWindow(const char* title, uint16_t width, uint16_t height);

		void Show() override;

		void OnUpdate() override;

		void* GetNativeWindowHandle() override { return m_HWND; }

		HWND GetHandle();

	private:
		HWND m_HWND;
	};

}
