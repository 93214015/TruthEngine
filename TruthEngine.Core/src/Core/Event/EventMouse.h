#pragma once
#include "Event.h"
#include "Core/Input/MouseButtons.h"

namespace TruthEngine {

	class EventMouseMoved : public Event {

	public:
		EventMouseMoved(float x, float y) : m_MouseX(x), m_MouseY(y){}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "EventMoseMoved: X=" << m_MouseX << ", Y=" << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)

	private:
		float m_MouseX, m_MouseY;

	};


	class EventMouseScrolled : public Event {
	public:
		EventMouseScrolled(float offsetX, float offsetY) : m_OffsetX(offsetX), m_OffsetY(offsetY){}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "EventMouseScrolled: OffsetX=" << m_OffsetX << ", OffsetY=" << m_OffsetY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_OffsetX, m_OffsetY;
	};

	class EventMouseButton : public Event {
	public:
		MouseButton GetButtonCode() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse  | EventCategoryMouseButton | EventCategoryInput)

	protected:
		EventMouseButton(const MouseButton mouseCode) : m_Button(mouseCode)
		{}

		MouseButton m_Button;

	};


	class EventMouseButtonPressed : public EventMouseButton {

	public:
		EventMouseButtonPressed(const MouseButton buttonCode) : EventMouseButton(buttonCode){}

		std::string ToString() const {
			std::stringstream ss;
			ss << "EventMouseButtonPressed: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)

	private:

	};


	class EventMouseButtonReleased : public EventMouseButton
	{
	public:

		EventMouseButtonReleased(const MouseButton buttonCode) : EventMouseButton(buttonCode){}

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "EventMouseButtonReleased: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)

	private:
	};

}
