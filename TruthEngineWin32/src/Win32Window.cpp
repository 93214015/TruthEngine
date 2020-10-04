#include "pch.h"
#include "Win32Window.h"
#include "TruthEngineApp.h"

std::unique_ptr<TruthEngine::Core::Window> TruthEngine::Core::CreateWindow(const char* title, uint16_t width, uint16_t height)
{
	return std::make_unique<Win32Window>(title, width, height);
}

TruthEnigne::Win32::Win32Window::Win32Window(const char* title, uint16_t width, uint16_t height) : TruthEngine::Core::Window(title, width, height)
{
	auto App = TruthEngine::Core::TruthEngineApp::GetApplication();

	const auto hInstance = GetModuleHandle(NULL);

	WNDCLASSEXA wndclass = { 0 };
	wndclass.cbSize = sizeof(WNDCLASSEXA);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.hInstance = hInstance;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = m_Title.c_str();
	RegisterClassExA(&wndclass);

	RECT rect = { 0, 0, static_cast<LONG>(App->GetClientWidth()), static_cast<LONG>(App->GetClientHeight()) };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	m_HWND = CreateWindowA(
		wndclass.lpszClassName
		, m_Title.c_str()
		, WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, rect.right - rect.left
		, rect.bottom - rect.top
		, nullptr
		, nullptr
		, hInstance
		, nullptr);
}

void TruthEnigne::Win32::Win32Window::OnUpdate()
{
	
}

void TruthEnigne::Win32::Win32Window::SetEventCallBack(const EventCallBack& fn)
{
	
}

HWND TruthEnigne::Win32::Win32Window::GetHandle()
{
	return m_HWND;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{

	case WM_CREATE:
	{
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
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
	return DefWindowProc(hwnd, message, wParam, lParam);
}