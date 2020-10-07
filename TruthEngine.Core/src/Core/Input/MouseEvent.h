#pragma once

#pragma once


namespace TruthEngine::Core {

	struct MousePoint
	{
		int x;
		int y;
	};

	class MouseEvent {
	public:
		enum EventType {
			LPress,
			LRelease,
			RPress,
			RRelease,
			MPress,
			MRelease,
			WheelUp,
			WheelDown,
			Move,
			Raw_Move,
			Invalid
		};

	private:
		EventType m_type;
		int m_x;
		int m_y;
	public:
		MouseEvent();
		MouseEvent(const EventType etype, const int x, const int y);
		bool IsValid() const;
		EventType GetType() const;
		MousePoint GetPos() const;
		int GetPosX() const;
		int GetPosY() const;
	};

}
