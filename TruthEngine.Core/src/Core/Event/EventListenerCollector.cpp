#include "pch.h"
#include "EventListenerCollector.h"
#include "Event.h"

namespace TruthEngine
{



	uint32_t EventListenerCollector::AddListener(const EventListener& _EventListener)
	{
		uint32_t _InsertedSlot = 0;

		if (!m_FreeSlotList.empty())
		{
			_InsertedSlot = m_FreeSlotList.front();
			m_Collection.emplace(m_Collection.cbegin() + _InsertedSlot, CollectionItem{ 1, _EventListener });
			m_FreeSlotList.pop();
		}
		else
		{
			_InsertedSlot = m_Collection.size();
			m_Collection.emplace_back(1, _EventListener);
		}

		return _InsertedSlot;
	}

	void EventListenerCollector::RemoveListener(uint32_t _ListenerSlot)
	{
		m_Collection[_ListenerSlot].Valid = 0;
		m_FreeSlotList.push(_ListenerSlot);
	}

	void EventListenerCollector::Invoke(class Event& _Event)
	{
		for (auto& _Item : m_Collection)
		{
			if (_Item.Valid == 1)
				_Item.Listener(_Event);

			if (_Event.Handled)
				break;
		}
	}

}