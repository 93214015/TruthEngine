#pragma once

namespace TruthEngine
{
	enum class TE_DEPTH_WRITE_MASK
	{
		ZERO = 0,
		ALL = 1
	};

	enum class TE_COMPARISON_FUNC
	{
		NEVER = 1,
		LESS = 2,
		EQUAL = 3,
		LESS_EQUAL = 4,
		GREATER = 5,
		NOT_EQUAL = 6,
		GREATER_EQUAL = 7,
		ALWAYS = 8
	};

	enum class TE_STENCIL_OP
	{
		KEEP = 1,
		ZERO = 2,
		REPLACE = 3,
		INCR_SAT = 4,
		DECR_SAT = 5,
		INVERT = 6,
		INCR = 7,
		DECR = 8
	};

	struct TE_DEPTH_STENCIL_OP_DESC
	{
		TE_DEPTH_STENCIL_OP_DESC() = default;
		TE_DEPTH_STENCIL_OP_DESC(TE_STENCIL_OP _FailOp, TE_STENCIL_OP _DepthFailOp, TE_STENCIL_OP _PassOp, TE_COMPARISON_FUNC _StencilFunc)
			: StencilFailOp(_FailOp), StencilDepthFailOp(_DepthFailOp), StencilPassOp(_PassOp), StencilFunc(_StencilFunc)
		{}

		TE_STENCIL_OP StencilFailOp = TE_STENCIL_OP::KEEP;
		TE_STENCIL_OP StencilDepthFailOp = TE_STENCIL_OP::KEEP;
		TE_STENCIL_OP StencilPassOp = TE_STENCIL_OP::KEEP;
		TE_COMPARISON_FUNC StencilFunc = TE_COMPARISON_FUNC::ALWAYS;

		operator D3D12_DEPTH_STENCILOP_DESC() const
		{
			return D3D12_DEPTH_STENCILOP_DESC
			{
				static_cast<D3D12_STENCIL_OP>(StencilFailOp),
				static_cast<D3D12_STENCIL_OP>(StencilDepthFailOp),
				static_cast<D3D12_STENCIL_OP>(StencilPassOp),
				static_cast<D3D12_COMPARISON_FUNC>(StencilFunc)
			};
		}
	};

	struct PipelineDepthStencilDesc
	{
		PipelineDepthStencilDesc() = default;
		PipelineDepthStencilDesc(TE_DEPTH_WRITE_MASK _DepthWriteMask, TE_COMPARISON_FUNC _DepthFunc
			, uint8_t _StencilReadMask = 0xff, uint8_t _StencilWriteMask = 0xff
			, const TE_DEPTH_STENCIL_OP_DESC& _FrontFace = TE_DEPTH_STENCIL_OP_DESC{}, const TE_DEPTH_STENCIL_OP_DESC& _BackFace = TE_DEPTH_STENCIL_OP_DESC{}, bool _DepthBoundsTestEnable = false)
			: DepthWriteMask(_DepthWriteMask), DepthFunc(_DepthFunc), StencilReadMask(_StencilReadMask)
			, StencilWriteMask(_StencilWriteMask), FrontFace(_FrontFace), BackFace(_BackFace), DepthBoundsTestEnable(_DepthBoundsTestEnable)
		{}

		TE_DEPTH_WRITE_MASK DepthWriteMask = TE_DEPTH_WRITE_MASK::ALL;
		TE_COMPARISON_FUNC DepthFunc = TE_COMPARISON_FUNC::LESS_EQUAL;
		uint8_t StencilReadMask = 0xff;
		uint8_t StencilWriteMask = 0xff;
		TE_DEPTH_STENCIL_OP_DESC FrontFace{};
		TE_DEPTH_STENCIL_OP_DESC BackFace{};
		bool DepthBoundsTestEnable = false;

		operator D3D12_DEPTH_STENCIL_DESC1() const
		{
			return D3D12_DEPTH_STENCIL_DESC1
			{
				true,
				static_cast<D3D12_DEPTH_WRITE_MASK>(DepthWriteMask),
				static_cast<D3D12_COMPARISON_FUNC>(DepthFunc),
				false,
				StencilReadMask,
				StencilWriteMask,
				static_cast<D3D12_DEPTH_STENCILOP_DESC>(FrontFace),
				static_cast<D3D12_DEPTH_STENCILOP_DESC>(BackFace),
				DepthBoundsTestEnable
			};
		}

		operator D3D12_DEPTH_STENCIL_DESC() const
		{
			return D3D12_DEPTH_STENCIL_DESC
			{
				true,
				static_cast<D3D12_DEPTH_WRITE_MASK>(DepthWriteMask),
				static_cast<D3D12_COMPARISON_FUNC>(DepthFunc),
				false,
				StencilReadMask,
				StencilWriteMask,
				static_cast<D3D12_DEPTH_STENCILOP_DESC>(FrontFace),
				static_cast<D3D12_DEPTH_STENCILOP_DESC>(BackFace),
			};
		}


	};

}
