#include "pch.h"
#include "RenderPass.h"

namespace TruthEngine
{
    RenderPass::RenderPass(TE_IDX_RENDERPASS idx, RendererLayer* _RendererLayer)
        : m_RenderPassIDX(idx), m_RendererLayer(_RendererLayer)
    {
    }
}