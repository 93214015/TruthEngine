#pragma once
#include "MouseButtons.h"

namespace TruthEngine::Core {

	struct MousePoint
	{
		int x;
		int y;
	};

	class Mouse
	{
	public:
		void OnLeftPressed(int x, int y);
		void OnLeftReleased(int x, int y);
		void OnRightPressed(int x, int y);
		void OnRightReleased(int x, int y);
		void OnMiddlePressed(int x, int y);
		void OnMiddleReleased(int x, int y);
		void OnWheelUp(int x, int y);
		void OnWheelDown(int x, int y);
		void OnMouseMove(int x, int y);
		void OnMouseMoveRaw(int x, int y);

		bool IsLeftDown() const;
		bool IsRightDown() const;
		bool IsMiddleDown() const;

		int GetPosX() const;
		int GetPosY() const;
		int GetDX() const;
		int GetDY() const;
		MousePoint GetPos() const;


	private:

		bool b_leftIsDown = false;
		bool b_rightIsDown = false;
		bool b_middleIsDown = false;

		int m_X = 0;
		int m_Y = 0;
		int m_LastX = 0;
		int m_LastY = 0;

	};

}
