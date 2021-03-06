#include "pch.h"
#include "InputManager.h"

#include "Core/Application.h"
#include "Core/Event/EventKey.h"
#include "Core/Event/EventMouse.h"

namespace TruthEngine {


	void InputManager::OnMouseMove(int x, int y)
	{
		if (x > 0 && y > 0)
		{
			m_MouseLastX = m_MouseX;
			m_MouseLastY = m_MouseY;
			m_MouseX = x;
			m_MouseY = y;
			auto event = EventMouseMoved(static_cast<float>(x), static_cast<float>(y));
			TE_INSTANCE_APPLICATION->OnEvent(event);
		}
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

	void InputManager::ProcessInput()
	{
		for (auto key : m_RegisteredKeys)
		{
			if (IsKeyPressed(key))
			{
				EventKeyPressed event{key, 0};
				TE_INSTANCE_APPLICATION->OnEvent(event);
			}
		}
	}

	void InputManager::RegisterKey(const KeyCode key)
	{
		if (std::find(m_RegisteredKeys.begin(), m_RegisteredKeys.end(), key) == m_RegisteredKeys.end())
		{
			m_RegisteredKeys.push_back(key);
		}
	}

	void InputManager::UnRegisterKey(const KeyCode key)
	{
		auto itr = std::find(m_RegisteredKeys.begin(), m_RegisteredKeys.end(), key);
		if (itr != m_RegisteredKeys.end())
		{
			m_RegisteredKeys.erase(itr);
		}
	}

	void InputManager::OnKeyPressed(const KeyCode key)
	{
		auto event = TruthEngine::EventKeyPressed(key, 0);
		TE_INSTANCE_APPLICATION->OnEvent(event);
	}

	void InputManager::OnKeyReleased(const KeyCode key)
	{
		auto event = TruthEngine::EventKeyReleased(key);
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

	std::list<TruthEngine::KeyCode> InputManager::m_RegisteredKeys;

	int InputManager::m_MouseX, InputManager::m_MouseY, InputManager::m_MouseLastX, InputManager::m_MouseLastY;

	


}