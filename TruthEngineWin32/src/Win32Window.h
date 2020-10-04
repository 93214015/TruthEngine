#pragma once
#include "Core/Window.h"

namespace TruthEnigne::Win32 {


	class Win32Window : public TruthEngine::Core::Window {


	public:
		Win32Window(const char* title, uint16_t width, uint16_t height);

		void OnUpdate() override;

		void SetEventCallBack(const EventCallBack& fn) override;

		HWND GetHandle();

	private:


	private:
		HWND m_HWND;

	};


}