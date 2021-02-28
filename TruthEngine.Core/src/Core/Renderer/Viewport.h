#pragma once

namespace TruthEngine
{

	struct Viewport {
		Viewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
			: TopLeftX(topLeftX), TopLeftY(topLeftY), Width(width), Height(height), MinDepth(minDepth), MaxDepth(maxDepth)
		{}

		void Resize(float width, float height)
		{
			Width = width;
			Height = height;
		}

		float TopLeftX;
		float TopLeftY;
		float Width;
		float Height;
		float MinDepth;
		float MaxDepth;
	};

	struct ViewRect
	{
		ViewRect(long left, long top, long right, long bottom)
			:Left(left), Top(top), Right(right), Bottom(bottom)
		{}

		long    Left;
		long    Top;
		long    Right;
		long    Bottom;
	};
}
