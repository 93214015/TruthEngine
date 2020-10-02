#include "pch.h"
#include "Mouse.h"

namespace TruthEngine::Core {

	void Mouse::OnLeftPressed(int x, int y)
	{
		b_leftIsDown = true;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::LPress, x, y));
	}

	void Mouse::OnLeftReleased(int x, int y)
	{
		b_leftIsDown = false;

		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::LRelease, x, y));
	}

	void Mouse::OnRightPressed(int x, int y)
	{
		b_rightIsDown = true;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::RPress, x, y));
	}

	void Mouse::OnRightReleased(int x, int y)
	{
		b_rightIsDown = false;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::RRelease, x, y));
	}

	void Mouse::OnMiddlePressed(int x, int y)
	{
		b_middleIsDown = true;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::MPress, x, y));
	}

	void Mouse::OnMiddleReleased(int x, int y)
	{
		b_middleIsDown = false;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::MRelease, x, y));
	}

	void Mouse::OnWheelUp(int x, int y)
	{
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y));
	}

	void Mouse::OnWheelDown(int x, int y)
	{
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y));
	}

	void Mouse::OnMouseMove(int x, int y)
	{
		mLastX = mX;
		mLastY = mY;
		mX = x;
		mY = y;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::Move, x, y));

		for (auto& func : mListenersOnMove)
		{
			func(this);
		}
	}

	void Mouse::OnMouseMoveRaw(int x, int y)
	{
		mX = x;
		mY = y;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::Raw_Move, x, y));
	}

	bool Mouse::IsLeftDown() const
	{
		return b_leftIsDown;
	}

	bool Mouse::IsRightDown() const
	{
		return b_rightIsDown;
	}

	bool Mouse::IsMiddleDown() const
	{
		return b_middleIsDown;
	}

	int Mouse::GetPosX() const
	{
		return mX;
	}

	int Mouse::GetPosY() const
	{
		return mY;
	}

	int Mouse::GetDX() const
	{
		return mX - mLastX;
	}

	int Mouse::GetDY() const
	{
		return mY - mLastY;
	}

	MousePoint Mouse::GetPos() const
	{
		return { mX, mY };
	}

	bool Mouse::IsEventBufferEmpty() const
	{
		return m_eventBuffer.empty();
	}

	MouseEvent Mouse::PopEvent()
	{
		if (m_eventBuffer.empty())
			return MouseEvent();

		auto e = m_eventBuffer.front();
		m_eventBuffer.pop();
		return e;
	}

	MouseEvent* Mouse::ReadEvent()
	{
		if (m_eventBuffer.empty())
			return nullptr;

		auto& e = m_eventBuffer.front();
		return &e;
	}

}
