#include "pch.h"
#include "Core/Input/InputManager.h"


#ifdef TE_PLATFORM_WINDOWS

	namespace TruthEngine::Core
	{

		bool InputManager::IsKeyPressed(const TruthEngine::Core::KeyCode key)
		{
			return GetAsyncKeyState(key) & 0x8000;
		}

	}

#endif