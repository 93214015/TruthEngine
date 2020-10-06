#pragma once
#include "Core/Window.h"

namespace TruthEngine::Platforms::Windows {

	class WindowWindows : public TruthEngine::Core::Window
	{
	public:
		WindowWindows(const char* title, uint16_t width, uint16_t height);

		void Show() override;

		void OnUpdate() override;

		HWND GetHandle();

	private:
		HWND m_HWND;
	};

}
