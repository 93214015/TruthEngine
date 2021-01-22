#include "pch.h"
#include "Pipeline.h"

TruthEngine::Core::Pipeline::Pipeline(RendererStateSet states, Shader* shader, uint32_t renderTargetNum, TE_RESOURCE_FORMAT* rtvFormat, float depthBias, float depthBiasClamp, float slopeScaledDepthBias)
	: m_States(states), m_Shader(shader), m_RenderTargetNum(renderTargetNum), m_DepthBias(depthBias), m_DepthBiasClamp(depthBiasClamp), m_SlopeScaledDepthBias(slopeScaledDepthBias)
{
	if (renderTargetNum > 0)
		memcpy(m_RTVFormats, rtvFormat, sizeof(TE_RESOURCE_FORMAT) * renderTargetNum);
}
