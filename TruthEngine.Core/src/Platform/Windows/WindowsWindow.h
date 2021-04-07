#pragma once
#include "Core/Window.h"

namespace TruthEngine::Platforms::Windows {

	class WindowsWindow : public TruthEngine::Window
	{
	public:
		WindowsWindow(const char* title, uint16_t width, uint16_t height);

		void Show() override;

		void OnUpdate() override;

		void* GetNativeWindowHandle() override { return m_HWND; }

		HWND GetHandle();

		void ToggleFullScreenMode() override;

	private:
		HWND m_HWND;

		RECT m_WindowRect;
		UINT m_WindowStyle = WS_OVERLAPPEDWINDOW;

	};

}
