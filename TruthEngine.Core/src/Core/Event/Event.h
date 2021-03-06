#pragma once


namespace TruthEngine {

	enum class EventType : uint8_t {
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, SceneViewportResize,
		AppTick, AppUpdate, AppProcess, AppOneSecondPoint,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		EntityAddMesh, EntityAddMaterial, EntityUpdatedMaterial, EntityAddLight, EntityUpdatedLight, EntityTransform,
		RendererNewGraphicsPipeline, RendererNewComputePipeline, RendererViewportResize, RendererTextureResize,
		SettingsGraphicsHDR, SettingsGraphicsFrameLimit,
		EventTypeNum
	};

	enum EventCategory : uint16_t {
		None = 0,
		EventCategoryApplication =		BIT(0),
		EventCategoryInput =			BIT(1),
		EventCategoryKeyboard =			BIT(2),
		EventCategoryMouse =			BIT(3),
		EventCategoryMouseButton =		BIT(4),
		EventCategoryEntity =			BIT(5),
		EventCategoryRenderer =			BIT(6),
		EventCategorySettings =			BIT(7),
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


	

}
