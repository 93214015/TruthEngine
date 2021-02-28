#include "pch.h"
#include "Pipeline.h"

TruthEngine::Pipeline::Pipeline(RendererStateSet states, Shader* shader, uint32_t renderTargetNum, TE_RESOURCE_FORMAT* rtvFormat, TE_RESOURCE_FORMAT dsvFormat, bool enableMSAA, float depthBias, float depthBiasClamp, float slopeScaledDepthBias)
	: m_States(states), m_Shader(shader), m_RenderTargetNum(renderTargetNum), m_DepthBias(depthBias), m_DepthBiasClamp(depthBiasClamp), m_SlopeScaledDepthBias(slopeScaledDepthBias), m_DSVFormat(dsvFormat), m_EnableMSAA(enableMSAA)
{
	if (renderTargetNum > 0)
		memcpy(m_RTVFormats, rtvFormat, sizeof(TE_RESOURCE_FORMAT) * renderTargetNum);
}
