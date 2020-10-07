#include "pch.h"
#include "Event.h"

namespace TruthEngine::Core {
   

    void EventDispatcher::RegisterListener(const EventType eventType, const EventListener& eventFunc)
    {
        m_EventListerns[static_cast<int>(eventType)].push_back(eventFunc);
    }

	void EventDispatcher::OnEvent(const Event& event)
	{
        for (auto& eventFunc : m_EventListerns[static_cast<int>(event.GetEventType())])
        {
            eventFunc(event);
        }        
	}

}