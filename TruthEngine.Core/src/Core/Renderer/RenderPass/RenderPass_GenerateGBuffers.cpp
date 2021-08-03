#include "pch.h"
#include "RenderPass_GenerateGBuffers.h"

#include "Core/Renderer/RendererLayer.h"

#include "Core/Event/EventApplication.h"

namespace TruthEngine
{
	RenderPass_GenerateGBuffers::RenderPass_GenerateGBuffers(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATEGBUFFERS, _RendererLayer)
		, m_Viewport{0.0f, 0.0f, static_cast<float>(TE_INSTANCE_APPLICATION->GetSceneViewportWidth()), static_cast<float>(TE_INSTANCE_APPLICATION->GetSceneViewportHeight()), 0.0f, 1.0f}
		, m_ViewRect{0L, 0L, static_cast<long>(TE_INSTANCE_APPLICATION->GetSceneViewportWidth()), static_cast<long>(TE_INSTANCE_APPLICATION->GetSceneViewportHeight())}
	{
	}

	void RenderPass_GenerateGBuffers::OnAttach()
	{

		m_BufferManager = TE_INSTANCE_BUFFERMANAGER;
		m_ShaderManager = TE_INSTANCE_SHADERMANAGER;

		m_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATEGBUFFERS, TE_IDX_SHADERCLASS::GENERATEGBUFFERS, m_BufferManager, m_ShaderManager);

		InitTextures();
		InitBuffers();
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

		m_Viewport.Resize(static_cast<float>(_Application->GetSceneViewportWidth()), static_cast<float>(_Application->GetSceneViewportHeight()));
		m_ViewRect = ViewRect{ 0L, 0L, static_cast<long>(_Application->GetSceneViewportWidth()), static_cast<long>(_Application->GetSceneViewportHeight()) };

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

	void RenderPass_GenerateGBuffers::InitBuffers()
	{
		m_ConstantBufferDirect_PerMesh = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>(TE_IDX_GRESOURCES::CBuffer_PerMesh_GBuffers);
	}

	void RenderPass_GenerateGBuffers::PreparePipelines(const Material* _Material)
	{
		
	}

	void RenderPass_GenerateGBuffers::OnRenderTargetResize(const EventTextureResize& _Event)
	{

		if (_Event.GetIDX() != TE_IDX_GRESOURCES::Texture_RT_BackBuffer)
			return;

		uint32_t _NewWidth = _Event.GetWidth();
		uint32_t _NewHeight = _Event.GetHeight();

		m_Viewport.Resize(static_cast<float>(_NewWidth), static_cast<float>(_NewHeight));
		m_ViewRect = ViewRect{ 0L, 0L, static_cast<long>(_NewWidth), static_cast<long>(_NewHeight) };

		m_RendererCommand.ResizeRenderTarget(m_TextureGBufferColor, _NewWidth, _NewHeight, &m_RenderTargetViewGBufferColor, nullptr);
		m_RendererCommand.ResizeRenderTarget(m_TextureGBufferNormal, _NewWidth, _NewHeight, &m_RenderTargetViewGBufferNormal, nullptr);

	}
}