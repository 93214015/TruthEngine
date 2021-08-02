#include "pch.h"
#include "DirectX12PipelineManager.h"

#include "Core/Renderer/Pipeline.h"
#include "Core/Renderer/Shader.h"

#include "API/DX12/DirectX12GraphicDevice.h"
#include "API/DX12/DirectX12ShaderManager.h"
#include "API/DX12/DirectX12Manager.h"

#include "Core/Event/EventRenderer.h"

namespace TruthEngine::API::DirectX12
{

	static std::mutex g_MutexPipelineManager;

	constexpr D3D12_CULL_MODE DX12_GET_CULL_MODE(RendererStateSet states)
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

	constexpr D3D12_DEPTH_WRITE_MASK DX12_GET_DEPTH_WRITE_MASK(RendererStateSet states)
	{
		TE_RENDERER_STATE_DEPTH_WRITE_MASK _DepthWrite = static_cast<TE_RENDERER_STATE_DEPTH_WRITE_MASK>(GET_RENDERER_STATE(states, TE_RENDERER_STATE_DEPTH_WRITE_MASK));

		return static_cast<D3D12_DEPTH_WRITE_MASK>(_DepthWrite);
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

	struct TED3D12GraphicsPipelineStateDesc
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC D3D12Desc = { 0 };

		TED3D12GraphicsPipelineStateDesc(const PipelineGraphics* _Pipeline)
		{
			const Shader* shader = _Pipeline->GetShader();
			const RendererStateSet	states = _Pipeline->GetStates();


			D3D12Desc.VS = CD3DX12_SHADER_BYTECODE(shader->GetVS().BufferPointer, shader->GetVS().BufferSize);
			D3D12Desc.PS = CD3DX12_SHADER_BYTECODE(shader->GetPS().BufferPointer, shader->GetPS().BufferSize);
			D3D12Desc.HS = CD3DX12_SHADER_BYTECODE(shader->GetHS().BufferPointer, shader->GetHS().BufferSize);
			D3D12Desc.DS = CD3DX12_SHADER_BYTECODE(shader->GetDS().BufferPointer, shader->GetDS().BufferSize);
			D3D12Desc.GS = CD3DX12_SHADER_BYTECODE(shader->GetGS().BufferPointer, shader->GetGS().BufferSize);

			//Blend Desc
			D3D12Desc.BlendState = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT());

			D3D12Desc.SampleMask = UINT_MAX;

			//Rasterized Desc
			D3D12Desc.RasterizerState.CullMode = DX12_GET_CULL_MODE(states);
			D3D12Desc.RasterizerState.FillMode = DX12_GET_FILL_MODE(states);
			D3D12Desc.RasterizerState.FrontCounterClockwise = DX12_GET_FRONTCOUNTERCLOCKWISE(states);
			D3D12Desc.RasterizerState.DepthBias = _Pipeline->GetDepthBias();
			D3D12Desc.RasterizerState.DepthBiasClamp = _Pipeline->GetDepthBiasClamp();
			D3D12Desc.RasterizerState.SlopeScaledDepthBias = _Pipeline->GetSlopeScaledDepthBias();
			D3D12Desc.RasterizerState.DepthClipEnable = true;
			D3D12Desc.RasterizerState.MultisampleEnable = true;
			D3D12Desc.RasterizerState.AntialiasedLineEnable = false;
			D3D12Desc.RasterizerState.ForcedSampleCount = 0;
			D3D12Desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

			//Blend Desc
			auto depthEnabled = DX12_GET_ENABLED_DEPTH(states);
			D3D12Desc.DepthStencilState.DepthEnable = depthEnabled;
			D3D12Desc.DepthStencilState.DepthWriteMask = DX12_GET_DEPTH_WRITE_MASK(states);
			D3D12Desc.DepthStencilState.DepthFunc = depthEnabled ? DX12_GET_COMPARISON_FUNC(states) : D3D12_COMPARISON_FUNC_ALWAYS;
			D3D12Desc.DepthStencilState.StencilEnable = false;
			D3D12Desc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
			D3D12Desc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
			D3D12Desc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			D3D12Desc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			D3D12Desc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			D3D12Desc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			D3D12Desc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			D3D12Desc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			D3D12Desc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			D3D12Desc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

			//Input Layout
			DX12_GET_INPUT_ELEMENTS(*shader->GetInputElements(), m_InputElements);
			D3D12Desc.InputLayout.NumElements = static_cast<UINT>(m_InputElements.size());
			D3D12Desc.InputLayout.pInputElementDescs = m_InputElements.data();

			D3D12Desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
			D3D12Desc.PrimitiveTopologyType = DX12_GET_PRIMITIVE_TOPOLOGY_TYPE(states);
			D3D12Desc.NumRenderTargets = _Pipeline->GetRenderTargetNum();
			DX12_GET_RTV_FORMATS(_Pipeline->GetRenderTargetFormats(), D3D12Desc.RTVFormats);
			D3D12Desc.DSVFormat = DX12_GET_FORMAT(_Pipeline->GetDSVFormat());

			//Sample desc
			D3D12Desc.SampleDesc.Count = _Pipeline->GetEnableMSAA() ? static_cast<uint32_t>(Settings::MSAA) : 1;
			D3D12Desc.SampleDesc.Quality = 0;

			D3D12Desc.pRootSignature = DirectX12Manager::GetInstance()->GetD3D12RootSignature(shader->GetShaderClassIDX());

			D3D12Desc.NodeMask = 0;
			D3D12Desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		}

	private:
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElements;

	};

	struct TED3D12ComputePipelineStateDesc
	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC D3D12Desc = { 0 };

		TED3D12ComputePipelineStateDesc(const PipelineCompute* _Pipeline)
		{
			Shader* _Shader = _Pipeline->GetShader();
			D3D12Desc.CS = CD3DX12_SHADER_BYTECODE(_Shader->GetCS().BufferPointer, _Shader->GetCS().BufferSize);
			D3D12Desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
			D3D12Desc.NodeMask = 0;

			D3D12Desc.pRootSignature = DirectX12Manager::GetInstance()->GetD3D12RootSignature(_Shader->GetShaderClassIDX());
		}
	};

	COMPTR<ID3D12PipelineState> DirectX12PiplineManager::GetGraphicsPipeline(PipelineGraphics* pipeline)
	{

		if (auto _ItrPipelineState = m_MapPipelineState.find(pipeline->GetID()); _ItrPipelineState != m_MapPipelineState.end())
		{
			return _ItrPipelineState->second;
		}

		COMPTR<ID3D12PipelineState> PSO;

		//
		////Get Pipeline Desc
		//
		TED3D12GraphicsPipelineStateDesc _TED3D12PipelineGraphicsDesc{ pipeline };

		auto hr = m_PiplineLibrary->LoadGraphicsPipeline(to_wstring(pipeline->m_Name).c_str(), &_TED3D12PipelineGraphicsDesc.D3D12Desc, IID_PPV_ARGS(PSO.GetAddressOf()));

		if (FAILED(hr))
		{
			std::scoped_lock<std::mutex> _ScopedLock(g_MutexPipelineManager);
			hr = m_PiplineLibrary->LoadGraphicsPipeline(to_wstring(pipeline->m_Name).c_str(), &_TED3D12PipelineGraphicsDesc.D3D12Desc, IID_PPV_ARGS(PSO.GetAddressOf()));

			if (FAILED(hr))
			{
				AddGraphicsPipeline(pipeline, PSO, _TED3D12PipelineGraphicsDesc);
			}
		}

		return PSO;
	}

	COMPTR<ID3D12PipelineState> DirectX12PiplineManager::GetComputePipeline(PipelineCompute* pipeline)
	{
		if (auto _ItrPipelineState = m_MapPipelineState.find(pipeline->GetID()); _ItrPipelineState != m_MapPipelineState.end())
		{
			return _ItrPipelineState->second;
		}

		COMPTR<ID3D12PipelineState> PSO;


		//
		////CreatePipline Desc
		//

		TED3D12ComputePipelineStateDesc _TEDesc{ pipeline };

		auto _HR = m_PiplineLibrary->LoadComputePipeline(to_wstring(pipeline->m_Name).c_str(), &_TEDesc.D3D12Desc, IID_PPV_ARGS(PSO.GetAddressOf()));

		if (FAILED(_HR))
		{
			AddComputePipeline(pipeline, PSO, _TEDesc);
		}

		return PSO;
	}

	TE_RESULT DirectX12PiplineManager::AddGraphicsPipeline(PipelineGraphics* pipeline, COMPTR<ID3D12PipelineState>& PSO, const TED3D12GraphicsPipelineStateDesc& _Desc)
	{
		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateGraphicsPipelineState(&_Desc.D3D12Desc, IID_PPV_ARGS(PSO.GetAddressOf()));
		TE_ASSERT_CORE(SUCCEEDED(hr), "PipelineGraphics Creation failed!");

		hr = m_PiplineLibrary->StorePipeline(to_wstring(pipeline->m_Name).c_str(), PSO.Get());

		m_MapPipelineState[pipeline->GetID()] = PSO;

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_FAIL;
	}

	TE_RESULT DirectX12PiplineManager::AddComputePipeline(PipelineCompute* pipeline, COMPTR<ID3D12PipelineState>& PSO, const TED3D12ComputePipelineStateDesc& _Desc)
	{
		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateComputePipelineState(&_Desc.D3D12Desc, IID_PPV_ARGS(PSO.GetAddressOf()));
		TE_ASSERT_CORE(SUCCEEDED(hr), "PipelineCompute Creation failed!");

		hr = m_PiplineLibrary->StorePipeline(to_wstring(pipeline->m_Name).c_str(), PSO.Get());

		m_MapPipelineState[pipeline->GetID()] = PSO;

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_FAIL;
	}

	TE_RESULT DirectX12PiplineManager::OnEventNewPipelineGraphics(const EventRendererNewGraphicsPipeline& _Event)
	{
		const PipelineGraphics* _Pipeline = _Event.GetPipeline();

		TED3D12GraphicsPipelineStateDesc _TEDesc{ _Pipeline };

		COMPTR<ID3D12PipelineState> _PSO;
		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateGraphicsPipelineState(&_TEDesc.D3D12Desc, IID_PPV_ARGS(_PSO.GetAddressOf()));

		TE_ASSERT_CORE(SUCCEEDED(hr), "PipelineGraphics Creation failed!");

		hr = m_PiplineLibrary->StorePipeline(to_wstring(_Pipeline->m_Name).c_str(), _PSO.Get());

		m_MapPipelineState[_Pipeline->GetID()] = _PSO;

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_FAIL;
	}

	TE_RESULT DirectX12PiplineManager::OnEventNewPipelineCompute(const EventRendererNewComputePipeline& _Event)
	{
		const PipelineCompute* _Pipeline = _Event.GetPipeline();

		TED3D12ComputePipelineStateDesc _TEDesc{_Pipeline};

		COMPTR<ID3D12PipelineState> _PSO;
		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateComputePipelineState(&_TEDesc.D3D12Desc, IID_PPV_ARGS(_PSO.GetAddressOf()));

		TE_ASSERT_CORE(SUCCEEDED(hr), "PipelineCompute Creation failed!");

		hr = m_PiplineLibrary->StorePipeline(to_wstring(_Pipeline->m_Name).c_str(), _PSO.Get());

		m_MapPipelineState[_Pipeline->GetID()] = _PSO;

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_FAIL;
	}

	DirectX12PiplineManager::DirectX12PiplineManager()
	{
		TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreatePipelineLibrary(nullptr, 0, IID_PPV_ARGS(m_PiplineLibrary.ReleaseAndGetAddressOf()));

		//////////////////////////////////////////////////////////////////////
		//Register The EventListener for new Pipelines
		//////////////////////////////////////////////////////////////////////
		auto _OnEventNewPipelineGraphics = [this](Event& _Event)
		{
			OnEventNewPipelineGraphics(static_cast<EventRendererNewGraphicsPipeline&>(_Event));
		};

		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererNewGraphicsPipeline, _OnEventNewPipelineGraphics);


		auto _OnEventNewPipelineCompute = [this](Event& _Event)
		{
			OnEventNewPipelineCompute(static_cast<EventRendererNewComputePipeline&>(_Event));
		};

		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererNewComputePipeline, _OnEventNewPipelineCompute);

		//////////////////////////////////////////////////////////////////////


	}

}