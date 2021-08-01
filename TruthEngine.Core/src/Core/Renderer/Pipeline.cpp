#include "pch.h"
#include "Pipeline.h"

#include "Core/Renderer/Shader.h"

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

	PipelineGraphics::PipelineGraphics(uint32_t ID, std::string_view _Name, RendererStateSet states, Shader* shader, uint32_t renderTargetNum, TE_RESOURCE_FORMAT* rtvFormat, TE_RESOURCE_FORMAT dsvFormat, bool enableMSAA, float depthBias, float depthBiasClamp, float slopeScaledDepthBias)
		: m_ID(ID), m_Name(_Name), m_States(states), m_Shader(shader), m_ShaderClassIDX(shader->GetShaderClassIDX()), m_RenderTargetNum(renderTargetNum), m_DepthBias(depthBias), m_DepthBiasClamp(depthBiasClamp), m_SlopeScaledDepthBias(slopeScaledDepthBias), m_DSVFormat(dsvFormat), m_EnableMSAA(enableMSAA)
	{
		if (renderTargetNum > 0)
			memcpy(m_RTVFormats, rtvFormat, sizeof(TE_RESOURCE_FORMAT) * renderTargetNum);
	}

	void PipelineGraphics::Factory(PipelineGraphics** _outPipeline, RendererStateSet states, Shader* shader, uint32_t renderTargetNum, TE_RESOURCE_FORMAT* rtvFormat, TE_RESOURCE_FORMAT dsvFormat, bool enableMSAA, float depthBias, float depthBiasClamp, float slopeScaledDepthBias)
	{
		static PipelineGraphicsContainer sPipelineContainer;

		std::string _Name = "PSO_";
		_Name += std::to_string(gPipelineID);

		if (*_outPipeline == nullptr)
		{
			*_outPipeline = &sPipelineContainer.mPipelines.emplace_back(PipelineGraphics(
				gPipelineID++,
				_Name,
				states,
				shader,
				renderTargetNum,
				rtvFormat,
				dsvFormat,
				enableMSAA,
				depthBias,
				depthBiasClamp,
				slopeScaledDepthBias
			));
		}
		else
		{
			**_outPipeline = PipelineGraphics(
				(*_outPipeline)->m_ID,
				_Name,
				states,
				shader,
				renderTargetNum,
				rtvFormat,
				dsvFormat,
				enableMSAA,
				depthBias,
				depthBiasClamp,
				slopeScaledDepthBias);
		}

		EventRendererNewGraphicsPipeline _Event{ *_outPipeline };
		
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

	void PipelineCompute::Factory(PipelineCompute** _outPipeline, Shader* _Shader)
	{
		static PipelineComputeContainer sContainer;

		std::string _Name = "PSO_";
		_Name += std::to_string(gPipelineID);

		if (*_outPipeline == nullptr)
		{
			*_outPipeline = &sContainer.mPipelines.emplace_back(PipelineCompute(gPipelineID++, _Name, _Shader));
		}
		else
		{
			**_outPipeline = PipelineCompute((*_outPipeline)->m_ID, (*_outPipeline)->m_Name, _Shader);
		}

		EventRendererNewComputePipeline _Event{ *_outPipeline };

		TE_INSTANCE_APPLICATION->OnEvent(_Event);
	}

	PipelineCompute::PipelineCompute(uint32_t _ID, std::string_view _Name, Shader* _Shader)
		: m_ID(_ID), m_Name(_Name), m_Shader(_Shader), m_ShaderClassIDX(_Shader->GetShaderClassIDX())
	{}

	PipelineCompute::~PipelineCompute() = default;

	PipelineCompute::PipelineCompute(const PipelineCompute& _Pipeline) = default;

	PipelineCompute& PipelineCompute::operator=(const PipelineCompute& _Pipeline) = default;

	PipelineCompute::PipelineCompute(PipelineCompute&& _Pipeline) noexcept = default;

	PipelineCompute& PipelineCompute::operator=(PipelineCompute&& _Pipeline) noexcept = default;

}
