#include "pch.h"
#include "Event.h"

namespace TruthEngine {
   

    EventListenerHandle EventDispatcher::RegisterListener(const EventType eventType, const EventListener& eventFunc)
    {
        uint32_t _EventIndex = static_cast<uint32_t>(eventType);
        EventListenerHandle _Handle{ uint2{_EventIndex, static_cast<uint32_t>(m_EventListerns[_EventIndex].size())} };
        m_EventListerns[_EventIndex].push_back(eventFunc);
        return _Handle;
    }

    void EventDispatcher::UnRegisterListener(EventListenerHandle* _Handles, size_t _HandleCount)
    {
        for (size_t i = 0; i < _HandleCount; i++)
        {
            auto& _ListenerVector = m_EventListerns[_Handles[i].m_Indecies.x];
            _ListenerVector.erase(_ListenerVector.begin() + _Handles[i].m_Indecies.y);
        }
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