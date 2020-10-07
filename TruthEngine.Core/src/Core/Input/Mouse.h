#pragma once
#include "MouseEvent.h"

namespace TruthEngine::Core {

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

		bool IsEventBufferEmpty() const;
		MouseEvent PopEvent();
		MouseEvent* ReadEvent();

	private:

		std::queue<MouseEvent> m_eventBuffer;
		bool b_leftIsDown = false;
		bool b_rightIsDown = false;
		bool b_middleIsDown = false;

		int mX = 0;
		int mY = 0;
		int mLastX = 0;
		int mLastY = 0;

	};

}
