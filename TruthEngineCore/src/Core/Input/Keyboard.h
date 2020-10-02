#pragma once

#include "KeyCodes.h"
#include "KeyboardEvent.h"

namespace TruthEngine::Core {

	class Keyboard
	{
	public:
		Keyboard();
		bool KeyIsPressed(const KeyCode keycode) const;
		bool KeyBufferIsEmpty() const;
		bool CharBufferIsEmpty() const;
		KeyboardEvent PopKey();
		KeyboardEvent* ReadKey();
		unsigned char PopChar();
		unsigned char* ReadChar();
		void OnKeyPressed(const KeyCode key);
		void OnKeyReleased(const KeyCode key);
		void OnChar(const unsigned char key);
		void EnableAutoRepeatKeys();
		void DisableAutoRepeatKeys();
		void EnableAutoRepeatChars();
		void DisableAutoRepeatChars();
		bool IsKeysAutoRepeat() const;
		bool IsCharsAutoRepeat() const;

	private:
		bool b_autoRepeatKeys;
		bool b_autoRepeatChars;
		bool b_keyStates[348];

		std::queue<KeyboardEvent> m_keyBuffer;
		std::queue<unsigned char> m_charBuffer;


	};

}

