#pragma once

#include "Event.h"
#include "Core/Input/KeyCodes.h"

namespace TruthEngine::Core {

	class EventKey : public Event {


	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard);

	protected:
		EventKey(const KeyCode keyCode): m_KeyCode(keyCode){}

		KeyCode m_KeyCode;

	};

	class EventKeyPressed : public EventKey {

	public:
		EventKeyPressed(KeyCode keyCode, uint16_t repeatCode) : EventKey(keyCode), m_RepeatCount(repeatCode)
		{}

		uint16_t GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "EventKeyPressed: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	protected:
		uint16_t m_RepeatCount;
	};


	class EventKeyReleased : public EventKey {

	public:

		EventKeyReleased(KeyCode keyCode) : EventKey(keyCode){}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "EventKeyReleased: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)

	};


}
