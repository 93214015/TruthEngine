#include "pch.h"

#ifdef TE_PLATFORM_WINDOWS

#include "WindowsWindow.h"
#include "Core/Event/EventApplication.h"

#include "Core/Input/InputManager.h"

#include "Core/Renderer/SwapChain.h"
#include "API/DX12/DirectX12SwapChain.h"

#include <wrl/wrappers/corewrappers.h>

using namespace TruthEngine;

//forward declaration of ImGui function for processing window events.
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TruthEngine::Window::WindowData* windowData = nullptr;

	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
	{
		return true;
	}

	switch (message)
	{

	case WM_CREATE:
	{
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));

		windowData = reinterpret_cast<TruthEngine::Window::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	return 0;


	case WM_ACTIVATEAPP:
		return 0;

	case WM_SIZE:
		windowData->Width = LOWORD(lParam);
		windowData->Height = HIWORD(lParam);
		windowData->CallBack(TruthEngine::EventWindowResize{ LOWORD(lParam), HIWORD(lParam) });
		break;


	case WM_MOUSEMOVE:
		TruthEngine::InputManager::OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	case WM_INPUT:
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
	case WM_SYSKEYUP:
		//KeyboardObject::ProcessWNDMessages(*DirectXApplication::GetKeyboard(), message, wParam, lParam);
		return 0;

	case WM_KEYUP:
		InputManager::OnKeyReleased((KeyCode)wParam);
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

	WindowsWindow::WindowsWindow(const char* title, uint16_t width, uint16_t height) : Window(title, width, height)
	{

		HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		TE_ASSERT_CORE(SUCCEEDED(hr), "Windows CoInitialize() is failed");

		const auto hInstance = GetModuleHandle(NULL);

		WNDCLASSEXA wndclass = { 0 };
		wndclass.cbSize = sizeof(WNDCLASSEXA);
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = ::WindowProc;
		wndclass.hInstance = hInstance;
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.lpszClassName = title;
		RegisterClassExA(&wndclass);

		m_WindowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		AdjustWindowRect(&m_WindowRect, WS_OVERLAPPEDWINDOW, FALSE);

		m_HWND = CreateWindowA(
			wndclass.lpszClassName
			, title
			, m_WindowStyle
			, CW_USEDEFAULT
			, CW_USEDEFAULT
			, m_WindowRect.right - m_WindowRect.left
			, m_WindowRect.bottom - m_WindowRect.top
			, nullptr
			, nullptr
			, hInstance
			, &m_Data);

	}

	void WindowsWindow::OnUpdate()
	{
		MSG msg{};

		if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		if (msg.message == WM_QUIT)
			m_Data.CallBack(TruthEngine::EventWindowClose());

	}

	HWND WindowsWindow::GetHandle()
	{
		return m_HWND;
	}

	void WindowsWindow::Show()
	{
		ShowWindow(m_HWND, SW_SHOW);
	}

	void WindowsWindow::ToggleFullScreenMode()
	{
		auto windowMode = TE_INSTANCE_APPLICATION->GetWindowMode();

		if (!windowMode)
		{
			// Restore the window's attributes and size.
			SetWindowLong(m_HWND, GWL_STYLE, m_WindowStyle);

			SetWindowPos(
				m_HWND,
				HWND_NOTOPMOST,
				m_WindowRect.left,
				m_WindowRect.top,
				m_WindowRect.right - m_WindowRect.left,
				m_WindowRect.bottom - m_WindowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			ShowWindow(m_HWND, SW_NORMAL);
		}
		else
		{
			// Save the old window rect so we can restore it when exiting fullscreen mode.
			GetWindowRect(m_HWND, &m_WindowRect);

			// Make the window borderless so that the client area can fill the screen.
			SetWindowLong(m_HWND, GWL_STYLE, m_WindowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

			RECT fullscreenWindowRect;

			auto _swapChain = TE_INSTANCE_API_DX12_SWAPCHAIN.GetNativeObject();

			// Get the settings of the display on which the app's window is currently displayed
			COMPTR<IDXGIOutput> pOutput;
			_swapChain->GetContainingOutput(&pOutput);
			DXGI_OUTPUT_DESC Desc;
			pOutput->GetDesc(&Desc);
			fullscreenWindowRect = Desc.DesktopCoordinates;


			SetWindowPos(
				m_HWND,
				HWND_TOPMOST,
				fullscreenWindowRect.left,
				fullscreenWindowRect.top,
				fullscreenWindowRect.right,
				fullscreenWindowRect.bottom,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);


			ShowWindow(m_HWND, SW_MAXIMIZE);
		}

		TE_INSTANCE_APPLICATION->SetWindowMode(!windowMode);
	}

}


std::unique_ptr<TruthEngine::Window> TruthEngine::TECreateWindow(const char* title, uint32_t width, uint32_t height)
{
	return std::make_unique<TruthEngine::Platforms::Windows::WindowsWindow>(title, width, height);
}

#endif

