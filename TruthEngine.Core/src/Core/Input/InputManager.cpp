#include "pch.h"
#include "InputManager.h"

#include "Application.h"
#include "Core/Event/EventKey.h"
#include "Core/Event/EventMouse.h"

namespace TruthEngine::Core {


	void InputManager::OnMouseMove(int x, int y)
	{
		m_Mouse.OnMouseMove(x, y);
		auto event = EventMouseMoved(x, y);
		TE_INSTANCE_APPLICATION.OnEvent(event);
	}

	void InputManager::OnMouseLeftPressed(int x, int y)
	{
		m_Mouse.OnLeftPressed(x, y);
		auto event = EventMouseButtonPressed(MouseButtons::ButtonLeft);
		TE_INSTANCE_APPLICATION.OnEvent(event);
	}

	void InputManager::OnMouseLeftReleased(int x, int y)
	{
		m_Mouse.OnLeftReleased(x, y);
		auto event = EventMouseButtonReleased(MouseButtons::ButtonLeft);
		TE_INSTANCE_APPLICATION.OnEvent(event);
	}

	void InputManager::OnMouseRightPressed(int x, int y)
	{
		m_Mouse.OnRightPressed(x, y);
		auto event = EventMouseButtonPressed(MouseButtons::ButtonRight);
		TE_INSTANCE_APPLICATION.OnEvent(event);
	}

	void InputManager::OnMouseRightReleased(int x, int y)
	{
		m_Mouse.OnRightReleased(x, y);
		auto event = EventMouseButtonReleased(MouseButtons::ButtonRight);
		TE_INSTANCE_APPLICATION.OnEvent(event);
	}

	void InputManager::OnMouseMiddlePressed(int x, int y)
	{
		m_Mouse.OnMiddlePressed(x, y);
		auto event = EventMouseButtonPressed(MouseButtons::ButtonMiddle);
		TE_INSTANCE_APPLICATION.OnEvent(event);
	}

	void InputManager::OnMouseMiddleReleased(int x, int y)
	{
		m_Mouse.OnMiddleReleased(x, y);
		auto event = EventMouseButtonReleased(MouseButtons::ButtonMiddle);
		TE_INSTANCE_APPLICATION.OnEvent(event);
	}

	void InputManager::OnMouseWheelDown(int x, int y)
	{
		m_Mouse.OnWheelDown(x, y);
		auto event = EventMouseScrolled(x, y);
		TE_INSTANCE_APPLICATION.OnEvent(event);
	}

	void InputManager::OnMouseWheelUp(int x, int y)
	{
		m_Mouse.OnWheelUp(x, y);
		auto event = EventMouseScrolled(x, y);
		TE_INSTANCE_APPLICATION.OnEvent(event);
	}

	void InputManager::OnKeyPressed(const KeyCode key)
	{
		auto event = TruthEngine::Core::EventKeyPressed(key, 0);
		TE_INSTANCE_APPLICATION.OnEvent(event);
	}

	void InputManager::OnKeyReleased(const KeyCode key)
	{
		auto event = TruthEngine::Core::EventKeyReleased(key);
		TE_INSTANCE_APPLICATION.OnEvent(event);
	}

	bool InputManager::IsMouseLeftDown()
	{
		return m_Mouse.IsLeftDown();
	}

	bool InputManager::IsMouseRightDown()
	{
		return m_Mouse.IsRightDown();
	}

	bool InputManager::IsMouseMiddleDown()
	{
		return m_Mouse.IsMiddleDown();
	}

	int InputManager::GetPosX()
	{
		return m_Mouse.GetPosX();
	}

	int InputManager::GetPosY()
	{
		return m_Mouse.GetPosY();
	}

	int InputManager::GetDX()
	{
		return m_Mouse.GetDX();
	}

	int InputManager::GetDY()
	{
		return m_Mouse.GetDY();
	}

	MousePoint InputManager::GetPos()
	{
		return m_Mouse.GetPos();
	}


	


}