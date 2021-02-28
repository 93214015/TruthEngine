#include "pch.h"
#include "Core/Input/InputManager.h"


#ifdef TE_PLATFORM_WINDOWS

	namespace TruthEngine
	{

		bool InputManager::IsKeyPressed(const TruthEngine::KeyCode key)
		{
			return GetAsyncKeyState(key) & 0x8000;
		}

		bool InputManager::IsMouseLeftDown()
		{
			return GetAsyncKeyState(MouseButtons::ButtonLeft) & 0x8000;
		}

		bool InputManager::IsMouseRightDown()
		{
			return GetAsyncKeyState(MouseButtons::ButtonRight) & 0x8000;
		}

		bool InputManager::IsMouseMiddleDown()
		{
			return GetAsyncKeyState(MouseButtons::ButtonMiddle) & 0x8000;
		}

	}

#endif