#include "pch.h"
#include "Pipeline.h"

#include "Core/Event/EventRenderer.h"

uint32_t gPipelineID = 0;

namespace TruthEngine
{

	struct PipelineGraphicsContainer
	{
		PipelineGraphicsContainer()
		{
			mPipelines.reserve(1000);
		}
		std::vector<PipelineGraphics> mPipelines;
	};

	PipelineGraphics::PipelineGraphics(uint32_t ID, std::string_view _Name, RendererStateSet states, const ShaderHandle& shaderHandle, uint32_t renderTargetNum, const TE_RESOURCE_FORMAT* rtvFormat, TE_RESOURCE_FORMAT dsvFormat, const std::vector<ShaderInputElement>& inputElements, bool enableMSAA, const PipelineBlendDesc& pipelineBlendDesc, const PipelineDepthStencilDesc& pipelineDepthStencilDesc, float depthBias, float depthBiasClamp, float slopeScaledDepthBias)
		: m_ID(ID), m_Name(_Name), m_States(states), m_ShaderHandle(shaderHandle), m_ShaderClassIDX(shaderHandle.GetShader()->GetShaderClassIDX()), m_RenderTargetNum(renderTargetNum), m_ShaderInputElements(inputElements), m_DepthBias(depthBias), m_DepthBiasClamp(depthBiasClamp), m_SlopeScaledDepthBias(slopeScaledDepthBias), m_DSVFormat(dsvFormat), m_PipelineBlendMode(pipelineBlendDesc), m_PipelineDepthStencilDesc(pipelineDepthStencilDesc), m_EnableMSAA(enableMSAA)
	{
		if (renderTargetNum > 0)
			memcpy(m_RTVFormats, rtvFormat, sizeof(TE_RESOURCE_FORMAT) * renderTargetNum);
	}

	const Shader* PipelineGraphics::GetShader() const noexcept
	{
		return m_ShaderHandle.GetShader();
	}

	void PipelineGraphics::SetShader(const ShaderHandle& shaderHandle) noexcept
	{
		m_ShaderHandle = shaderHandle;
		m_ShaderClassIDX = shaderHandle.GetShader()->GetShaderClassIDX();
	}

	const std::vector<ShaderInputElement>& PipelineGraphics::GetShaderInputElements() const
	{
		return m_ShaderInputElements;
	}

	void PipelineGraphics::Factory(PipelineGraphics* _outPipeline, RendererStateSet states, const ShaderHandle& shaderHandle, uint32_t renderTargetNum, const TE_RESOURCE_FORMAT* rtvFormat, TE_RESOURCE_FORMAT dsvFormat, const std::vector<ShaderInputElement>& inputElements, bool enableMSAA, const PipelineBlendDesc& pipelineBlendDesc, const PipelineDepthStencilDesc& pipelineDepthStencilDesc, float depthBias, float depthBiasClamp, float slopeScaledDepthBias)
	{
		//static PipelineGraphicsContainer sPipelineContainer;


		if (_outPipeline->m_ID == -1)
		{
			std::string _Name = "PSO_";
			_Name += std::to_string(gPipelineID);

			*_outPipeline = PipelineGraphics(
				gPipelineID++,
				_Name,
				states,
				shaderHandle,
				renderTargetNum,
				rtvFormat,
				dsvFormat,
				inputElements,
				enableMSAA,
				pipelineBlendDesc,
				pipelineDepthStencilDesc,
				depthBias,
				depthBiasClamp,
				slopeScaledDepthBias
			);
		}
		else
		{
			*_outPipeline = PipelineGraphics(
				_outPipeline->m_ID,
				_outPipeline->m_Name,
				states,
				shaderHandle,
				renderTargetNum,
				rtvFormat,
				dsvFormat,
				inputElements,
				enableMSAA,
				pipelineBlendDesc,
				pipelineDepthStencilDesc,
				depthBias,
				depthBiasClamp,
				slopeScaledDepthBias);
		}

		EventRendererNewGraphicsPipeline _Event{ _outPipeline };

		TE_INSTANCE_APPLICATION->OnEvent(_Event);

	}

	struct PipelineComputeContainer
	{
		PipelineComputeContainer()
		{
			mPipelines.reserve(50);
		}

		std::vector<PipelineCompute> mPipelines;
	};

	void PipelineCompute::Factory(PipelineCompute* _outPipeline, const ShaderHandle& _ShaderHandle)
	{
		//static PipelineComputeContainer sContainer;

		std::string _Name = "PSO_";
		_Name += std::to_string(gPipelineID);

		if (_outPipeline->m_ID == -1)
		{
			*_outPipeline = PipelineCompute(gPipelineID++, _Name, _ShaderHandle);
		}
		else
		{
			*_outPipeline = PipelineCompute(_outPipeline->m_ID, _outPipeline->m_Name, _ShaderHandle);
		}

		EventRendererNewComputePipeline _Event{ _outPipeline };

		TE_INSTANCE_APPLICATION->OnEvent(_Event);
	}

	PipelineCompute::PipelineCompute(uint32_t _ID, std::string_view _Name, const ShaderHandle& _ShaderHandle)
		: m_ID(_ID), m_Name(_Name), m_ShaderHandle(_ShaderHandle), m_ShaderClassIDX(_ShaderHandle.GetShader()->GetShaderClassIDX())
	{}

	const Shader* PipelineCompute::GetShader() const noexcept
	{
		return m_ShaderHandle.GetShader();
	}
}
