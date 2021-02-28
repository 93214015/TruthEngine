#pragma once
#include "Mouse.h"
#include "KeyCodes.h"

namespace TruthEngine {

	class InputManager {

	public:
		static void ProcessInput();

		static void RegisterKey(const KeyCode key);
		static void UnRegisterKey(const KeyCode key);

		static void OnKeyPressed(const KeyCode key);
		static void OnKeyReleased(const KeyCode key);

		static bool IsKeyPressed(const KeyCode key);

		static void OnMouseMove(int x, int y);
		static void OnMouseLeftPressed(int x, int y);
		static void OnMouseLeftReleased(int x, int y);
		static void OnMouseRightPressed(int x, int y);
		static void OnMouseRightReleased(int x, int y);
		static void OnMouseMiddlePressed(int x, int y);
		static void OnMouseMiddleReleased(int x, int y);
		static void OnMouseWheelDown(int x, int y);
		static void OnMouseWheelUp(int x, int y);
		static bool IsMouseLeftDown();
		static bool IsMouseRightDown();
		static bool IsMouseMiddleDown();
		static int GetPosX();
		static int GetPosY();
		static int GetDX();
		static int GetDY();
		static MousePoint GetPos();

	private:
		InputManager() = default;

	private:

		static std::list<KeyCode> m_RegisteredKeys;

		static int m_MouseX, m_MouseY, m_MouseLastX, m_MouseLastY;

		static Mouse m_Mouse;

	};

}
