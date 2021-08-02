#include "pch.h"
#include "RenderPass_GenerateGBuffers.h"

#include "Core/Renderer/RendererLayer.h"

namespace TruthEngine
{
	RenderPass_GenerateGBuffers::RenderPass_GenerateGBuffers(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATEGBUFFERS, _RendererLayer)
	{
	}

	void RenderPass_GenerateGBuffers::OnAttach()
	{
	}
	void RenderPass_GenerateGBuffers::OnDetach()
	{
	}
	void RenderPass_GenerateGBuffers::OnImGuiRender()
	{
	}
	void RenderPass_GenerateGBuffers::BeginScene()
	{
	}
	void RenderPass_GenerateGBuffers::EndScene()
	{
	}
	void RenderPass_GenerateGBuffers::Render()
	{
	}

	void RenderPass_GenerateGBuffers::InitTextures()
	{
		Application* _Application = TE_INSTANCE_APPLICATION;

		m_TextureGBufferColor = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Color
			, _Application->GetSceneViewportWidth()
			, _Application->GetSceneViewportHeight()
			, TE_RESOURCE_FORMAT::R8G8B8A8_UNORM
			, ClearValue_RenderTarget{ .0f, .0f, .0f, 1.0f }
			, true
			, Settings::IsMSAAEnabled);

		m_TextureGBufferNormal = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Normal
			, _Application->GetSceneViewportWidth()
			, _Application->GetSceneViewportHeight()
			, TE_RESOURCE_FORMAT::R11G11B10_FLOAT
			, ClearValue_RenderTarget{ .0f, .0f, .0f, 1.0f }
			, true
			, Settings::IsMSAAEnabled);

		m_RendererCommand.CreateRenderTargetView(m_TextureGBufferColor, &m_RenderTargetViewGBufferColor);
		m_RendererCommand.CreateRenderTargetView(m_TextureGBufferNormal, &m_RenderTargetViewGBufferNormal);
	}
}