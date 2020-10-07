#include "pch.h"
#include "Mouse.h"

namespace TruthEngine::Core {

	void Mouse::OnLeftPressed(int x, int y)
	{
		b_leftIsDown = true;
		
	}

	void Mouse::OnLeftReleased(int x, int y)
	{
		b_leftIsDown = false;

	}

	void Mouse::OnRightPressed(int x, int y)
	{
		b_rightIsDown = true;
	}

	void Mouse::OnRightReleased(int x, int y)
	{
		b_rightIsDown = false;
	}

	void Mouse::OnMiddlePressed(int x, int y)
	{
		b_middleIsDown = true;
	}

	void Mouse::OnMiddleReleased(int x, int y)
	{
		b_middleIsDown = false;
	}

	void Mouse::OnWheelUp(int x, int y)
	{
	}

	void Mouse::OnWheelDown(int x, int y)
	{
	}

	void Mouse::OnMouseMove(int x, int y)
	{
		m_LastX = m_X;
		m_LastY = m_Y;
		m_X = x;
		m_Y = y;
	}

	void Mouse::OnMouseMoveRaw(int x, int y)
	{
		m_X = x;
		m_Y = y;
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
		return m_X;
	}

	int Mouse::GetPosY() const
	{
		return m_Y;
	}

	int Mouse::GetDX() const
	{
		return m_X - m_LastX;
	}

	int Mouse::GetDY() const
	{
		return m_Y - m_LastY;
	}

	MousePoint Mouse::GetPos() const
	{
		return { m_X, m_Y };
	}

}
