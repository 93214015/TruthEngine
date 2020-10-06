#include "pch.h"
#include "Keyboard.h"


namespace TruthEngine::Core {

	Keyboard::Keyboard() : b_autoRepeatKeys(true), b_autoRepeatChars(false)
	{
		for (bool& b_keyState : b_keyStates)
		{
			b_keyState = false;
		}
	}

	bool Keyboard::KeyIsPressed(const KeyCode keycode) const
	{
		return b_keyStates[keycode];
	}

	bool Keyboard::KeyBufferIsEmpty() const
	{
		return m_keyBuffer.empty();
	}

	bool Keyboard::CharBufferIsEmpty() const
	{
		return m_charBuffer.empty();
	}

	KeyboardEvent Keyboard::PopKey()
	{
		if (m_keyBuffer.empty())
			return KeyboardEvent();

		KeyboardEvent& e = m_keyBuffer.front();
		m_keyBuffer.pop();
		return e;
	}

	KeyboardEvent* Keyboard::ReadKey()
	{
		if (m_keyBuffer.empty())
			return nullptr;

		KeyboardEvent& e = m_keyBuffer.front();
		return &e;
	}

	unsigned char Keyboard::PopChar()
	{
		if (m_charBuffer.empty())
			return 0u;

		unsigned char c = m_charBuffer.front();
		m_charBuffer.pop();
		return c;
	}

	unsigned char* Keyboard::ReadChar()
	{
		if (m_charBuffer.empty())
			return nullptr;

		return &m_charBuffer.front();
	}

	void Keyboard::OnKeyPressed(const KeyCode key)
	{
		b_keyStates[key] = true;
		m_keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
	}

	void Keyboard::OnKeyReleased(const KeyCode key)
	{
		b_keyStates[key] = false;
		m_keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
	}

	void Keyboard::OnChar(const unsigned char key)
	{
		m_charBuffer.push(key);
	}

	void Keyboard::EnableAutoRepeatKeys()
	{
		b_autoRepeatKeys = true;
	}

	void Keyboard::DisableAutoRepeatKeys()
	{
		b_autoRepeatKeys = false;
	}

	void Keyboard::EnableAutoRepeatChars()
	{
		b_autoRepeatChars = true;
	}

	void Keyboard::DisableAutoRepeatChars()
	{
		b_autoRepeatChars = false;
	}

	bool Keyboard::IsKeysAutoRepeat() const
	{
		return b_autoRepeatKeys;
	}

	bool Keyboard::IsCharsAutoRepeat() const
	{
		return b_autoRepeatChars;
	}

}