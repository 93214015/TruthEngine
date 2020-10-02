#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include "KeyCodes.h"

namespace TruthEngine::Core {

	class InputManager {

	public:
		static InputManager* Get() noexcept;


		void OnKeyPressed(const KeyCode key);
		void OnKeyReleased(const KeyCode key);
		bool IsKeyPressed(const KeyCode key) const;

		void OnMouseMove(int x, int y);
		void OnMouseLeftPressed(int x, int y);
		void OnMouseLeftReleased(int x, int y);
		void OnMouseRightPressed(int x, int y);
		void OnMouseRightReleased(int x, int y);
		void OnMouseMiddlePressed(int x, int y);
		void OnMouseMiddleReleased(int x, int y);
		void OnMouseWheelDown(int x, int y);
		void OnMouseWheelUp(int x, int y);
		bool IsMouseLeftDown() const;
		bool IsMouseRightDown() const;
		bool IsMouseMiddleDown() const;
		int GetPosX() const;
		int GetPosY() const;
		int GetDX() const;
		int GetDY() const;
		MousePoint GetPos() const;

	private:
		InputManager();

	private:
		static InputManager m_InputManager;

		Keyboard m_Keyboard;
		Mouse m_Mouse;

	};

}
