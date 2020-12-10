#include "pch.h"
#include "InputManager.h"

#include "Core/Application.h"
#include "Core/Event/EventKey.h"
#include "Core/Event/EventMouse.h"

namespace TruthEngine::Core {


	void InputManager::OnMouseMove(int x, int y)
	{
		m_MouseLastX = m_MouseX;
		m_MouseLastY = m_MouseY;
		m_MouseX = x;
		m_MouseY = y;
		auto event = EventMouseMoved(static_cast<float>(x), static_cast<float>(y));
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void InputManager::OnMouseLeftPressed(int x, int y)
	{
		auto event = EventMouseButtonPressed(MouseButtons::ButtonLeft);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void InputManager::OnMouseLeftReleased(int x, int y)
	{
		auto event = EventMouseButtonReleased(MouseButtons::ButtonLeft);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void InputManager::OnMouseRightPressed(int x, int y)
	{
		auto event = EventMouseButtonPressed(MouseButtons::ButtonRight);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void InputManager::OnMouseRightReleased(int x, int y)
	{
		auto event = EventMouseButtonReleased(MouseButtons::ButtonRight);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void InputManager::OnMouseMiddlePressed(int x, int y)
	{
		auto event = EventMouseButtonPressed(MouseButtons::ButtonMiddle);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void InputManager::OnMouseMiddleReleased(int x, int y)
	{
		auto event = EventMouseButtonReleased(MouseButtons::ButtonMiddle);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void InputManager::OnMouseWheelDown(int x, int y)
	{
		auto event = EventMouseScrolled(static_cast<float>(x), static_cast<float>(y));
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void InputManager::OnMouseWheelUp(int x, int y)
	{
		auto event = EventMouseScrolled(static_cast<float>(x), static_cast<float>(y));
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void InputManager::OnKeyPressed(const KeyCode key)
	{
		auto event = TruthEngine::Core::EventKeyPressed(key, 0);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void InputManager::OnKeyReleased(const KeyCode key)
	{
		auto event = TruthEngine::Core::EventKeyReleased(key);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	

	int InputManager::GetPosX()
	{
		return m_MouseX;
	}

	int InputManager::GetPosY()
	{
		return m_MouseY;
	}

	int InputManager::GetDX()
	{
		return m_MouseX - m_MouseLastX;
	}

	int InputManager::GetDY()
	{
		return m_MouseY - m_MouseLastY;
	}

	MousePoint InputManager::GetPos()
	{
		return MousePoint{m_MouseX, m_MouseY};
	}


	


}