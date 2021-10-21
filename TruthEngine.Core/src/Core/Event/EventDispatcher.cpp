#include "pch.h"
#include "EventDispatcher.h"

namespace TruthEngine
{
    EventListenerHandle EventDispatcher::RegisterListener(const EventType eventType, const EventListener& eventFunc)
    {
        uint32_t _EventTypeIndex = static_cast<uint32_t>(eventType);
        uint32_t _EventListenerIndex =  m_EventListerns[_EventTypeIndex].AddListener(eventFunc);
        return EventListenerHandle { uint2{_EventTypeIndex, _EventListenerIndex} };
    }

    void EventDispatcher::UnRegisterListener(EventListenerHandle* _Handles, size_t _HandleCount)
    {
        for (size_t i = 0; i < _HandleCount; i++)
        {
            m_EventListerns[_Handles[i].m_Indecies.x].RemoveListener(_Handles[i].m_Indecies.y);
        }
    }

    void EventDispatcher::OnEvent(Event& event)
    {
        m_EventListerns[static_cast<size_t>(event.GetEventType())].Invoke(event);
    }
}