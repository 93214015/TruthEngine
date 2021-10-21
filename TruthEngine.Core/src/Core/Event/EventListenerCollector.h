#pragma once

namespace TruthEngine
{
	using EventListener = std::function<void(class Event&)>;

	class EventListenerHandle
	{
		EventListenerHandle(uint2 _Indecies)
			: m_Indecies(_Indecies)
		{}
	private:
		uint2 m_Indecies;

		friend class EventDispatcher;
	};

	class EventListenerCollector
	{
	public:

		uint32_t AddListener(const EventListener& _EventListener);
		void RemoveListener(uint32_t _ListenerSlot);
		void Invoke(class Event& _Event);

	private:

	private:

		struct CollectionItem
		{
			CollectionItem() = default;
			CollectionItem(uint8_t _Valid, const EventListener& _Listener)
				: Valid(_Valid), Listener(_Listener)
			{}

			uint8_t Valid;
			EventListener Listener;
		};

		std::vector<CollectionItem> m_Collection;
		std::queue<uint32_t> m_FreeSlotList;

	};


}
