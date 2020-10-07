#pragma once
#include "Mouse.h"
#include "KeyCodes.h"

namespace TruthEngine::Core {

	class InputManager {

	public:
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

		static Mouse m_Mouse;

	};

}
