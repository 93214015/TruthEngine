#include "pch.h"
#include "DX12PipelineManager.h"

#include "Core/Renderer/Pipeline.h"
#include "Core/Renderer/Shader.h"

#include "API/DX12/DX12GraphicDevice.h"
#include "API/DX12/DX12ShaderManager.h"

namespace TruthEngine::API::DirectX12
{

	D3D12_CULL_MODE DX12_GET_CULL_MODE(RendererStateSet states)
	{
		TE_RENDERER_STATE_CULL_MODE cullMode = static_cast<TE_RENDERER_STATE_CULL_MODE>(GET_RENDERER_STATE(states, TE_RENDERER_STATE_CULL_MODE));

		switch (cullMode)
		{
		case TE_RENDERER_STATE_CULL_MODE_BACK:
			return D3D12_CULL_MODE_BACK;
		case TE_RENDERER_STATE_CULL_MODE_FRONT:
			return D3D12_CULL_MODE_FRONT;
		case TE_RENDERER_STATE_CULL_MODE_NONE:
			return D3D12_CULL_MODE_NONE;
		default:
			return D3D12_CULL_MODE_FRONT;
		}

		TE_ASSERT_CORE(false, "Wrong Cull Mode!");
	}

	D3D12_FILL_MODE DX12_GET_FILL_MODE(RendererStateSet states)
	{
		TE_RENDERER_STATE_FILL_MODE fillMode = static_cast<TE_RENDERER_STATE_FILL_MODE>(GET_RENDERER_STATE(states, TE_RENDERER_STATE_FILL_MODE));

		switch (fillMode)
		{
		case TE_RENDERER_STATE_FILL_MODE_WIREFRAME:
			return D3D12_FILL_MODE_WIREFRAME;
		case TE_RENDERER_STATE_FILL_MODE_SOLID:
			return D3D12_FILL_MODE_SOLID;
		default:
			return D3D12_FILL_MODE_SOLID;
		}

		TE_ASSERT_CORE(false, "Wrong Fill Mode!");
	}

	bool DX12_GET_FRONTCOUNTERCLOCKWISE(RendererStateSet states)
	{
		return static_cast<bool>(GET_RENDERER_STATE(states, TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE));
	}

	bool DX12_GET_ENABLED_DEPTH(RendererStateSet states)
	{
		return static_cast<bool>(GET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_DEPTH));
	}

	void DX12_GET_RTV_FORMATS(const TE_RESOURCE_FORMAT* inFormats, DXGI_FORMAT outFormats[8])
	{
		memcpy(outFormats, inFormats, sizeof(outFormats));
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE DX12_GET_PRIMITIVE_TOPOLOGY_TYPE(RendererStateSet states)
	{
		return static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(GET_RENDERER_STATE(states, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY));
	}

	inline DXGI_FORMAT DX12_GET_FORMAT(const TE_RESOURCE_FORMAT format)
	{
		return static_cast<DXGI_FORMAT>(format);
	}

	inline D3D12_INPUT_CLASSIFICATION DX12_GET_INPUT_CLASSIFICATION(TE_RENDERER_SHADER_INPUT_CLASSIFICATION inputClass)
	{
		return static_cast<D3D12_INPUT_CLASSIFICATION>(inputClass);
	}

	void DX12_GET_INPUT_ELEMENTS(const std::vector<Core::ShaderInputElement>& elements, std::vector<D3D12_INPUT_ELEMENT_DESC>& outElements)
	{
		outElements.reserve(elements.size());

		for (auto& el : elements)
		{
			outElements.emplace_back(
				D3D12_INPUT_ELEMENT_DESC{
				el.SemanticName.c_str(),
				el.SemanticIndex,
				DX12_GET_FORMAT(el.Format),
				el.InputSlot,
				el.AlignedByteOffset,
				DX12_GET_INPUT_CLASSIFICATION(el.InputSlotClass),
				el.InstanceDataStepRate
				}
			);
		}
	}

	constexpr int32_t DX12_DEFAULT_DEPTH_BIAS()
	{
		return 0;
	}

	constexpr float DX12_DEFAULT_DEPTH_BIAS_CLAMP()
	{
		return 0.0f;
	}

	constexpr float DX12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS()
	{
		return 0.0f;
	}


	D3D12_GRAPHICS_PIPELINE_STATE_DESC DX12PiplineManager::GetGraphicPipelineDesc(const Core::Pipeline* pipeline)
	{

		const auto shader = pipeline->GetShader();
		const auto states = pipeline->GetStates();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = { 0 };

		desc.VS = CD3DX12_SHADER_BYTECODE(shader->GetVS().BufferPointer, shader->GetVS().BufferSize);
		desc.PS = CD3DX12_SHADER_BYTECODE(shader->GetVS().BufferPointer, shader->GetVS().BufferSize);
		desc.HS = CD3DX12_SHADER_BYTECODE(shader->GetVS().BufferPointer, shader->GetVS().BufferSize);
		desc.DS = CD3DX12_SHADER_BYTECODE(shader->GetVS().BufferPointer, shader->GetVS().BufferSize);
		desc.GS = CD3DX12_SHADER_BYTECODE(shader->GetVS().BufferPointer, shader->GetVS().BufferSize);

		//Blend Desc
		desc.BlendState = CD3DX12_BLEND_DESC();

		//Rasterized Desc
		desc.RasterizerState.CullMode = DX12_GET_CULL_MODE(states);
		desc.RasterizerState.FillMode = DX12_GET_FILL_MODE(states);
		desc.RasterizerState.FrontCounterClockwise = DX12_GET_FRONTCOUNTERCLOCKWISE(states);
		desc.RasterizerState.DepthBias = DX12_DEFAULT_DEPTH_BIAS();
		desc.RasterizerState.DepthBiasClamp = DX12_DEFAULT_DEPTH_BIAS_CLAMP();
		desc.RasterizerState.SlopeScaledDepthBias = DX12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS();
		desc.RasterizerState.DepthClipEnable = true;
		desc.RasterizerState.MultisampleEnable = false;
		desc.RasterizerState.AntialiasedLineEnable = false;
		desc.RasterizerState.ForcedSampleCount = 0;
		desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		//Blend Desc
		desc.DepthStencilState.DepthEnable = DX12_GET_ENABLED_DEPTH(states);
		desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		desc.DepthStencilState.StencilEnable = false;
		desc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		desc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		desc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		desc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		//Input Layout
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;
		DX12_GET_INPUT_ELEMENTS(shader->GetInputElements(), inputElements);
		desc.InputLayout.NumElements = static_cast<UINT>(inputElements.size());
		desc.InputLayout.pInputElementDescs = inputElements.data();

		desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		desc.PrimitiveTopologyType = DX12_GET_PRIMITIVE_TOPOLOGY_TYPE(states);
		desc.NumRenderTargets = shader->GetRenderTargetNum();
		DX12_GET_RTV_FORMATS(pipeline->GetRenderTargetFormats(), desc.RTVFormats);
		desc.DSVFormat = DX12_GET_FORMAT(pipeline->GetDSVFormat());

		//Sample desc
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		desc.pRootSignature = TE_INSTANCE_API_DX12_SHADERMANAGER.GetRootSignature(shader);

		desc.NodeMask = 0;
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		return desc;
	}


	COMPTR<ID3D12PipelineState> DX12PiplineManager::GetPipeline(Core::Pipeline* pipeline)
	{
		COMPTR<ID3D12PipelineState> PSO;


		auto desc = GetGraphicPipelineDesc(pipeline);


		auto hr = m_PiplineLibrary->LoadGraphicsPipeline(to_wstring(pipeline->m_Name).c_str(), &desc, IID_PPV_ARGS(PSO.GetAddressOf()));


		if (FAILED(hr))
		{
			AddPipeline(pipeline, PSO, desc);
		}


		return PSO;
	}

	
	TE_RESULT DX12PiplineManager::AddPipeline(Core::Pipeline* pipeline, COMPTR<ID3D12PipelineState>& PSO, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
	{
		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(PSO.GetAddressOf()));
		if (FAILED(hr))
		{
			TE_ASSERT_CORE(false, "Pipeline Creation failed!");
		}
		pipeline->m_Name = std::string("PSO_") + std::to_string(m_PipelineNum);
		hr = m_PiplineLibrary->StorePipeline(to_wstring(pipeline->m_Name).c_str(), PSO.Get());

		pipeline->m_ID = m_PipelineNum++;

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_FAIL;
	}

	TruthEngine::API::DirectX12::DX12PiplineManager DX12PiplineManager::s_DX12PipelineManager;

}