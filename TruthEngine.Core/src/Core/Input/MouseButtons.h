#pragma once

namespace TruthEngine {

	using MouseButton = uint16_t;

	namespace MouseButtons
	{
		enum : MouseButton
		{
			// From glfw3.h
			Button0 = 0,
			Button1 = 1,
			Button2 = 2,
			Button3 = 3,
			Button4 = 4,
			Button5 = 5,
			Button6 = 6,
			Button7 = 7,

			ButtonLast = Button7,
			ButtonLeft = Button1,
			ButtonRight = Button2,
			ButtonMiddle = VK_MBUTTON
		};
	}

}
