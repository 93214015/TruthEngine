#include "pch.h"

#ifdef TE_PLATFORM_WINDOWS

#include "WindowWindows.h"
#include "Core/Event/EventApplication.h"

#include "Core/SwapChain.h"

//forward declaration of ImGui function for processing window events.
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TruthEngine::Core::Window::WindowData* windowData = nullptr;

	ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);

	switch (message)
	{

	case WM_CREATE:
	{
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));

		windowData = reinterpret_cast<TruthEngine::Core::Window::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	return 0;


	case WM_ACTIVATEAPP:
		return 0;

	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		//MouseObject::ProcessWNDMessage(*DirectXApplication::GetMouse(), message, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		//KeyboardObject::ProcessWNDMessages(*DirectXApplication::GetKeyboard(), message, wParam, lParam);
		return 0;

	case WM_SYSKEYDOWN:
		//KeyboardObject::ProcessWNDMessages(*DirectXApplication::GetKeyboard(), message, wParam, lParam);
		/*if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			...
		}*/
		return 0;


	case WM_PAINT:
		/*if (dxApp)
		{
			dxApp->OnUpdate();
			dxApp->OnRender();
		}*/
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		
	}

	//Handle any message that the switch statement didn't.
	return DefWindowProcA(hwnd, message, wParam, lParam);
}

namespace TruthEngine::Platforms::Windows {

	WindowWindows::WindowWindows(const char* title, uint16_t width, uint16_t height) : Window(title, width, height)
	{
	
		const auto hInstance = GetModuleHandle(NULL);

		WNDCLASSEXA wndclass = { 0 };
		wndclass.cbSize = sizeof(WNDCLASSEXA);
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = ::WindowProc;
		wndclass.hInstance = hInstance;
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.lpszClassName = title;
		RegisterClassExA(&wndclass);

		RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		m_HWND = CreateWindowA(
			wndclass.lpszClassName
			, title
			, WS_OVERLAPPEDWINDOW
			, CW_USEDEFAULT
			, CW_USEDEFAULT
			, rect.right - rect.left
			, rect.bottom - rect.top
			, nullptr
			, nullptr
			, hInstance
			, &m_Data);
	
	}

	void WindowWindows::OnUpdate()
	{
		MSG msg{};

		if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		if(msg.message == WM_QUIT)
			m_Data.CallBack(TruthEngine::Core::EventWindowClose());

		TE_INSTANCE_SWAPCHAIN->Present();

	}

	HWND WindowWindows::GetHandle()
	{
		return m_HWND;
	}

	void WindowWindows::Show()
	{
		ShowWindow(m_HWND, SW_SHOW);
	}

}


std::unique_ptr<TruthEngine::Core::Window> TruthEngine::Core::TECreateWindow(const char* title, uint32_t width, uint32_t height)
{
	return std::make_unique<TruthEngine::Platforms::Windows::WindowWindows>(title, width, height);
}

#endif

