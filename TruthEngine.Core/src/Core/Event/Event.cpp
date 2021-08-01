#include "pch.h"
#include "Event.h"

namespace TruthEngine {
   

    void EventDispatcher::RegisterListener(const EventType eventType, const EventListener& eventFunc)
    {
        m_EventListerns[static_cast<int>(eventType)].push_back(eventFunc);
    }

	void EventDispatcher::OnEvent(Event& event)
	{
        for (auto& eventFunc : m_EventListerns[static_cast<size_t>(event.GetEventType())])
        {
            eventFunc(event);

            if (event.Handled)
                break;
        }        
	}

}