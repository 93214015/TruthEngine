#pragma once


namespace TruthEngine::Core {

	enum class EventType : uint8_t {
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppProcess,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		EventTypeNum
	};

	enum EventCategory : uint16_t {
		None = 0,
		EventCategoryApplication =		BIT(0),
		EventCategoryInput =			BIT(1),
		EventCategoryKeyboard =			BIT(2),
		EventCategoryMouse =			BIT(3),
		EventCategoryMouseButton =		BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticEventType(){ return EventType::##type; }\
							   virtual EventType GetEventType() const override {return GetStaticEventType();}\
							   virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class Event {
	public:

		bool Handled = false;

		virtual ~Event() = default;

		virtual EventType GetEventType() const  = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

	protected:
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& event) {
		return os << event.ToString();
	}



	class EventDispatcher
	{
	public:
		using EventListener = std::function<void(const Event&)>;

		void RegisterListener(const EventType eventType, const EventListener& eventFunc);
		void OnEvent(const Event& event);

	protected:
		std::vector<EventListener> m_EventListerns[static_cast<uint8_t>(EventType::EventTypeNum)];

	};

}
