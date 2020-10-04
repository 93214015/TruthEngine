#include "pch.h"
#include "Win32Window.h"

void TruthEnigne::Win32::Win32Window::OnUpdate()
{
	
}

void TruthEnigne::Win32::Win32Window::SetEventCallBack(const EventCallBack& fn)
{
	
}

std::unique_ptr<TruthEngine::Core::Window> TruthEnigne::Win32::Win32Window::Create()
{
	return std::make_unique<TruthEngine::Core::Window>();
}
