#pragma once

namespace TruthEngine
{

	enum class TE_BLEND
	{
		ZERO = 1,
		ONE = 2,
		SRC_COLOR = 3,
		INV_SRC_COLOR = 4,
		SRC_ALPHA = 5,
		INV_SRC_ALPHA = 6,
		DEST_ALPHA = 7,
		INV_DEST_ALPHA = 8,
		DEST_COLOR = 9,
		INV_DEST_COLOR = 10,
		SRC_ALPHA_SAT = 11,
		BLEND_FACTOR = 14,
		INV_BLEND_FACTOR = 15,
		SRC1_COLOR = 16,
		INV_SRC1_COLOR = 17,
		SRC1_ALPHA = 18,
		INV_SRC1_ALPHA = 19
	};

	enum class TE_BLEND_OP
	{
		ADD = 1,
		SUBTRACT = 2,
		REV_SUBTRACT = 3,
		MIN = 4,
		MAX = 5
	};

	enum TE_COLOR_WRITE_ENABLE
	{
		TE_COLOR_WRITE_ENABLE_RED = 1,
		TE_COLOR_WRITE_ENABLE_GREEN = 2,
		TE_COLOR_WRITE_ENABLE_BLUE = 4,
		TE_COLOR_WRITE_ENABLE_ALPHA = 8,
		TE_COLOR_WRITE_ENABLE_ALL = (((TE_COLOR_WRITE_ENABLE_RED | TE_COLOR_WRITE_ENABLE_GREEN) | TE_COLOR_WRITE_ENABLE_BLUE) | TE_COLOR_WRITE_ENABLE_ALPHA)
	};


	struct PipelineBlendMode
	{
		PipelineBlendMode() = default;

		PipelineBlendMode(TE_BLEND _SourceBlend, TE_BLEND _DestinationBlend, TE_BLEND_OP _BlendingOperation
		, TE_BLEND _SourceBlendAlpha, TE_BLEND _DestinationBlendAlpha, TE_BLEND_OP _BlendingOperationAlpha
		, TE_COLOR_WRITE_ENABLE _RenderTargetWriteMask)
			: SourceBlend(_SourceBlend), DestinationBlend(_DestinationBlend), BlendingOperation(_BlendingOperation)
			, SourceBlendAlpha(_SourceBlendAlpha), DestinationBlendAlpha(_DestinationBlendAlpha), BlendingOperationAlpha(_BlendingOperationAlpha)
			, RenderTargetWriteMask(_RenderTargetWriteMask)
		{}

		TE_BLEND SourceBlend = TE_BLEND::ONE;
		TE_BLEND DestinationBlend = TE_BLEND::ZERO;
		TE_BLEND_OP BlendingOperation = TE_BLEND_OP::ADD;
		TE_BLEND SourceBlendAlpha = TE_BLEND::ONE;
		TE_BLEND DestinationBlendAlpha = TE_BLEND::ZERO;
		TE_BLEND_OP BlendingOperationAlpha = TE_BLEND_OP::ADD;
		uint8_t RenderTargetWriteMask = TE_COLOR_WRITE_ENABLE_ALL;
	};
}
