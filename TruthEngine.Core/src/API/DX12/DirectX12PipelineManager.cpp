#include "pch.h"
#include "DirectX12PipelineManager.h"

#include "Core/Renderer/Pipeline.h"
#include "Core/Renderer/Shader.h"

#include "API/DX12/DirectX12GraphicDevice.h"
#include "API/DX12/DirectX12ShaderManager.h"
#include "API/DX12/DirectX12Manager.h"

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
		auto topology = static_cast<D3D_PRIMITIVE_TOPOLOGY>(GET_RENDERER_STATE(states, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY));
		switch (topology)
		{
		case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
		case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case D3D_PRIMITIVE_TOPOLOGY_UNDEFINED:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		default:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		}
	}

	D3D12_COMPARISON_FUNC DX12_GET_COMPARISON_FUNC(RendererStateSet states)
	{
		return static_cast<D3D12_COMPARISON_FUNC>(GET_RENDERER_STATE(states, TE_RENDERER_STATE_COMPARISSON_FUNC));
	}

	inline DXGI_FORMAT DX12_GET_FORMAT(const TE_RESOURCE_FORMAT format)
	{
		return static_cast<DXGI_FORMAT>(format);
	}

	inline D3D12_INPUT_CLASSIFICATION DX12_GET_INPUT_CLASSIFICATION(TE_RENDERER_SHADER_INPUT_CLASSIFICATION inputClass)
	{
		return static_cast<D3D12_INPUT_CLASSIFICATION>(inputClass);
	}

	void DX12_GET_INPUT_ELEMENTS(const std::vector<ShaderInputElement>& elements, std::vector<D3D12_INPUT_ELEMENT_DESC>& outElements)
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



	COMPTR<ID3D12PipelineState> DirectX12PiplineManager::GetGraphicsPipeline(PipelineGraphics* pipeline)
	{
		COMPTR<ID3D12PipelineState> PSO;

		//
		////CreatePipline Desc
		//

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = { 0 };
		


		const auto shader = pipeline->GetShader();
		const auto states = pipeline->GetStates();


		desc.VS = CD3DX12_SHADER_BYTECODE(shader->GetVS().BufferPointer, shader->GetVS().BufferSize);
		desc.PS = CD3DX12_SHADER_BYTECODE(shader->GetPS().BufferPointer, shader->GetPS().BufferSize);
		desc.HS = CD3DX12_SHADER_BYTECODE(shader->GetHS().BufferPointer, shader->GetHS().BufferSize);
		desc.DS = CD3DX12_SHADER_BYTECODE(shader->GetDS().BufferPointer, shader->GetDS().BufferSize);
		desc.GS = CD3DX12_SHADER_BYTECODE(shader->GetGS().BufferPointer, shader->GetGS().BufferSize);

		//Blend Desc
		desc.BlendState = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT());

		desc.SampleMask = UINT_MAX;

		//Rasterized Desc
		desc.RasterizerState.CullMode = DX12_GET_CULL_MODE(states);
		desc.RasterizerState.FillMode = DX12_GET_FILL_MODE(states);
		desc.RasterizerState.FrontCounterClockwise = DX12_GET_FRONTCOUNTERCLOCKWISE(states);
		desc.RasterizerState.DepthBias = pipeline->m_DepthBias;
		desc.RasterizerState.DepthBiasClamp = pipeline->m_DepthBiasClamp;
		desc.RasterizerState.SlopeScaledDepthBias = pipeline->m_SlopeScaledDepthBias;
		desc.RasterizerState.DepthClipEnable = true;
		desc.RasterizerState.MultisampleEnable = true;
		desc.RasterizerState.AntialiasedLineEnable = false;
		desc.RasterizerState.ForcedSampleCount = 0;
		desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		//Blend Desc
		auto depthEnabled = DX12_GET_ENABLED_DEPTH(states);
		desc.DepthStencilState.DepthEnable = depthEnabled;
		desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthStencilState.DepthFunc = depthEnabled ? DX12_GET_COMPARISON_FUNC(states) : D3D12_COMPARISON_FUNC_ALWAYS;
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
		DX12_GET_INPUT_ELEMENTS(*shader->GetInputElements(), inputElements);
		desc.InputLayout.NumElements = static_cast<UINT>(inputElements.size());
		desc.InputLayout.pInputElementDescs = inputElements.data();

		desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		desc.PrimitiveTopologyType = DX12_GET_PRIMITIVE_TOPOLOGY_TYPE(states);
		desc.NumRenderTargets = pipeline->m_RenderTargetNum;
		DX12_GET_RTV_FORMATS(pipeline->GetRenderTargetFormats(), desc.RTVFormats);
		desc.DSVFormat = DX12_GET_FORMAT(pipeline->GetDSVFormat());

		//Sample desc
		desc.SampleDesc.Count = pipeline->m_EnableMSAA ? static_cast<uint32_t>(Settings::MSAA) : 1;
		desc.SampleDesc.Quality = 0;

		desc.pRootSignature = DirectX12Manager::GetInstance()->GetD3D12RootSignature(shader->GetShaderClassIDX());

		desc.NodeMask = 0;
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;


		auto hr = m_PiplineLibrary->LoadGraphicsPipeline(to_wstring(pipeline->m_Name).c_str(), &desc, IID_PPV_ARGS(PSO.GetAddressOf()));


		if (FAILED(hr))
		{
			AddGraphicsPipeline(pipeline, PSO, desc);
		}

		return PSO;
	}

	COMPTR<ID3D12PipelineState> DirectX12PiplineManager::GetComputePipeline(PipelineCompute* pipeline)
	{
		COMPTR<ID3D12PipelineState> PSO;

		Shader* _Shader = pipeline->GetShader();

		/*DirectX12ShaderManager _ShaderManager;
		auto _ShaderCode = _ShaderManager.CompileShader_OLD("shaderTest.hlsl", 200, "Assets/Shaders/HDR_DownScaling.hlsl", "HDRDownScalingFirstPass", "cs");*/

		//
		////CreatePipline Desc
		//


		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = { 0 };
		desc.CS = CD3DX12_SHADER_BYTECODE(_Shader->GetCS().BufferPointer, _Shader->GetCS().BufferSize);
		//desc.CS = CD3DX12_SHADER_BYTECODE(_ShaderCode.BufferPointer, _ShaderCode.BufferSize);
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.NodeMask = 0;

		desc.pRootSignature = DirectX12Manager::GetInstance()->GetD3D12RootSignature(_Shader->GetShaderClassIDX());

		auto _HR = m_PiplineLibrary->LoadComputePipeline(to_wstring(pipeline->m_Name).c_str(), &desc, IID_PPV_ARGS(PSO.GetAddressOf()));

		if (FAILED(_HR))
		{
			AddComputePipeline(pipeline, PSO, desc);
		}

		return PSO;
	}


	TE_RESULT DirectX12PiplineManager::AddGraphicsPipeline(PipelineGraphics* pipeline, COMPTR<ID3D12PipelineState>& PSO, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
	{
		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(PSO.GetAddressOf()));
		TE_ASSERT_CORE(SUCCEEDED(hr), "PipelineGraphics Creation failed!");

		pipeline->m_Name = std::string("PSO_") + std::to_string(m_PipelineNum);
		hr = m_PiplineLibrary->StorePipeline(to_wstring(pipeline->m_Name).c_str(), PSO.Get());

		m_PipelineNum++;

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_FAIL;
	}

	TE_RESULT DirectX12PiplineManager::AddComputePipeline(PipelineCompute* pipeline, COMPTR<ID3D12PipelineState>& PSO, D3D12_COMPUTE_PIPELINE_STATE_DESC& desc)
	{
		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateComputePipelineState(&desc, IID_PPV_ARGS(PSO.GetAddressOf()));
		TE_ASSERT_CORE(SUCCEEDED(hr), "PipelineCompute Creation failed!");

		pipeline->m_Name = std::string("PSO_") + std::to_string(m_PipelineNum);
		hr = m_PiplineLibrary->StorePipeline(to_wstring(pipeline->m_Name).c_str(), PSO.Get());

		m_PipelineNum++;

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_FAIL;
	}

	DirectX12PiplineManager::DirectX12PiplineManager()
	{
		TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreatePipelineLibrary(nullptr, 0, IID_PPV_ARGS(m_PiplineLibrary.ReleaseAndGetAddressOf()));
	}

}