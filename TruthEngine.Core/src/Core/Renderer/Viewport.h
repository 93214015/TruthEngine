#pragma once

namespace TruthEngine
{
	namespace Core {

		struct Viewport {
			float TopLeftX;
			float TopLeftY;
			float Width;
			float Height;
			float MinDepth;
			float MaxDepth;
		};

		struct ViewRect
		{
			long    left;
			long    top;
			long    right;
			long    bottom;
		};

	}
}
