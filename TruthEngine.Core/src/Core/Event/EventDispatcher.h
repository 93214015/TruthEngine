#pragma once

#include "Event.h"
#include "EventListenerCollector.h"

namespace TruthEngine
{
	

	class EventDispatcher
	{
	public:

		EventListenerHandle RegisterListener(const EventType eventType, const EventListener& eventFunc);
		void UnRegisterListener(EventListenerHandle* _Handles, size_t _HandleCount);
		void OnEvent(Event& event);

	protected:
		EventListenerCollector m_EventListerns[static_cast<uint8_t>(EventType::EventTypeNum)];

	};
}
