#pragma once
#include "Event.h"

namespace TruthEngine::Core {

	class EventWindowResize : public Event {
	public:
		EventWindowResize(const uint32_t width, const uint32_t height) : m_Width(width), m_Height(height){}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const {

			std::stringstream ss;
			ss << "EventWindowResize: width=" << m_Width << " ,height=" << m_Height;
			return ss.str();

		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		uint32_t m_Width, m_Height;
	};

	class EventSceneViewportResize : public Event
	{
	public:
		EventSceneViewportResize(const uint32_t width, const uint32_t height)
			: m_Width(width), m_Height(height)
		{}

		uint32_t GetWidth() const noexcept { return m_Width; }
		uint32_t GetHeight() const noexcept { return m_Height; }


		EVENT_CLASS_TYPE(SceneViewportResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	protected:
		uint32_t m_Width;
		uint32_t m_Height;

	};

	class EventTextureResize : public Event 
	{
	public:
		EventTextureResize(const uint16_t width, const uint16_t height, TE_IDX_TEXTURE texturedIDX)
			: m_Width(width), m_Height(height), m_TextureIDX(texturedIDX)
		{}

		uint32_t GetWidth() const noexcept { return m_Width; }
		uint32_t GetHeight() const noexcept { return m_Height; }
		TE_IDX_TEXTURE GetIDX()const noexcept { return m_TextureIDX; }


		EVENT_CLASS_TYPE(RenderTargetResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	protected:
		uint16_t m_Width, m_Height;
		TE_IDX_TEXTURE m_TextureIDX;
	};


	class EventWindowClose : public Event {
	public:
		EventWindowClose() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};


	class EventAppTick : public Event {
	public:
		EventAppTick() = default;

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class EventAppUpdate : public Event {
	public:
		EventAppUpdate() = default;

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class EventAppProcess : public Event {
	public:
		EventAppProcess() = default;

		EVENT_CLASS_TYPE(AppProcess)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

}