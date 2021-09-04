#include "pch.h"
#include "RenderPass.h"

namespace TruthEngine
{
    RenderPass::RenderPass(TE_IDX_RENDERPASS idx, RendererLayer* _RendererLayer)
        : m_RenderPassIDX(idx), m_RendererLayer(_RendererLayer)
    {
    }
    void RenderPass::OnAttach()
    {
        InitRendererCommand();
        InitTextures();
        InitBuffers();
        InitPipelines();

        RegisterEventListeners();
    }
    void RenderPass::OnDetach()
    {
        ReleaseTextures();
        ReleaseBuffers();
        ReleasePipelines();
        ReleaseRendererCommand();

        UnRegisterEventListeners();
    }
}