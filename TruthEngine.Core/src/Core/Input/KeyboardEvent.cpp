#include "pch.h"
#include "KeyboardEvent.h"

namespace TruthEngine::Core {

	KeyboardEvent::KeyboardEvent() : m_type(EventType::Invalid), m_key(0u)
	{

	}

	KeyboardEvent::KeyboardEvent(const EventType etype, const unsigned char key) : m_type(etype), m_key(key)
	{

	}

	bool KeyboardEvent::IsPress() const
	{
		return m_type == EventType::Press;
	}

	bool KeyboardEvent::IsReleased() const
	{
		return m_type == EventType::Release;
	}

	bool KeyboardEvent::IsValid() const
	{
		return m_type == EventType::Invalid;
	}

	unsigned char KeyboardEvent::GetkeyCode() const
	{
		return m_key;
	}

}