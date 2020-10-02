#pragma once

namespace TruthEngine::Core {

	class KeyboardEvent
	{
	public:
		enum EventType
		{
			Press,
			Release,
			Invalid
		};

		KeyboardEvent();
		KeyboardEvent(const EventType etype, const unsigned char key);
		bool IsPress() const;
		bool IsReleased() const;
		bool IsValid() const;
		unsigned char GetkeyCode() const;

	private:
		EventType m_type;
		unsigned char m_key;

	};

}
