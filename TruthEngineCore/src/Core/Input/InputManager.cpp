#include "pch.h"
#include "InputManager.h"

namespace TruthEngine::Core {

	InputManager* InputManager::Get() noexcept
	{
		return &m_InputManager;
	}

	bool InputManager::IsKeyPressed(const KeyCode key) const
	{
		return m_Keyboard.KeyIsPressed(key);
	}


	void InputManager::OnMouseMove(int x, int y)
	{
		m_Mouse.OnMouseMove(x, y);
	}

	void InputManager::OnMouseLeftPressed(int x, int y)
	{
		m_Mouse.OnLeftPressed(x, y);
	}

	void InputManager::OnMouseLeftReleased(int x, int y)
	{
		m_Mouse.OnLeftReleased(x, y);
	}

	void InputManager::OnMouseRightPressed(int x, int y)
	{
		m_Mouse.OnRightPressed(x, y);
	}

	void InputManager::OnMouseRightReleased(int x, int y)
	{
		m_Mouse.OnRightReleased(x, y);
	}

	void InputManager::OnMouseMiddlePressed(int x, int y)
	{
		m_Mouse.OnMiddlePressed(x, y);
	}

	void InputManager::OnMouseMiddleReleased(int x, int y)
	{
		m_Mouse.OnMiddleReleased(x, y);
	}

	void InputManager::OnMouseWheelDown(int x, int y)
	{
		m_Mouse.OnWheelDown(x, y);
	}

	void InputManager::OnMouseWheelUp(int x, int y)
	{
		m_Mouse.OnWheelUp(x, y);
	}

	void InputManager::OnKeyPressed(const KeyCode key)
	{
		m_Keyboard.OnKeyPressed(key);
	}

	void InputManager::OnKeyReleased(const KeyCode key)
	{
		m_Keyboard.OnKeyReleased(key);
	}

	bool InputManager::IsMouseLeftDown() const
	{
		return m_Mouse.IsLeftDown();
	}

	bool InputManager::IsMouseRightDown() const
	{
		return m_Mouse.IsRightDown();
	}

	bool InputManager::IsMouseMiddleDown() const
	{
		return m_Mouse.IsMiddleDown();
	}

	int InputManager::GetPosX() const
	{
		return m_Mouse.GetPosX();
	}

	int InputManager::GetPosY() const
	{
		return m_Mouse.GetPosY();
	}

	int InputManager::GetDX() const
	{
		return m_Mouse.GetDX();
	}

	int InputManager::GetDY() const
	{
		return m_Mouse.GetDY();
	}

	TruthEngine::MousePoint InputManager::GetPos() const
	{
		return m_Mouse.GetPos();
	}

	InputManager InputManager::m_InputManager;


	


}