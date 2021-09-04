#include "pch.h"
#include "RenderPass_DeferredShading.h"

#include "Core/Application.h"
#include "Core/Event/Event.h"

#include "Core/Renderer/ShaderManager.h"
#include "Core/Renderer/RendererLayer.h"

namespace TruthEngine
{
	RenderPass_DeferredShading::RenderPass_DeferredShading(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::DEFERREDSHADING, _RendererLayer)
	{}

	void RenderPass_DeferredShading::OnImGuiRender()
	{
	}
	void RenderPass_DeferredShading::OnUpdate(double _DeltaTime)
	{
	}
	void RenderPass_DeferredShading::BeginScene()
	{
		m_RendererCommand.BeginGraphics(&m_Pipeline);

		m_RendererCommand.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
		m_RendererCommand.SetRenderTarget(m_RendererLayer->GetRenderTargetViewSceneNoMS());
		
	}
	void RenderPass_DeferredShading::EndScene()
	{
		m_RendererCommand.End();
	}
	void RenderPass_DeferredShading::Render()
	{
		m_RendererCommand.ExecutePendingCommands();

		m_RendererCommand.Draw(4, 0);
	}
	void RenderPass_DeferredShading::InitRendererCommand()
	{
		m_RendererCommand.Init(
			TE_IDX_RENDERPASS::DEFERREDSHADING,
			TE_IDX_SHADERCLASS::DEFERREDSHADING,
			TE_INSTANCE_BUFFERMANAGER,
			TE_INSTANCE_SHADERMANAGER);
	}
	void RenderPass_DeferredShading::InitTextures()
	{
	}
	void RenderPass_DeferredShading::InitBuffers()
	{
	}
	void RenderPass_DeferredShading::ReleaseTextures()
	{
	}
	void RenderPass_DeferredShading::ReleaseBuffers()
	{
	}
	void RenderPass_DeferredShading::ReleasePipelines()
	{
	}
	void RenderPass_DeferredShading::ReleaseRendererCommand()
	{
		m_RendererCommand.Release();
	}
	void RenderPass_DeferredShading::InitPipelines()
	{
		RendererStateSet _RendererStates = InitRenderStates(
			TE_RENDERER_STATE_ENABLED_SHADER_HS_FALSE,
			TE_RENDERER_STATE_ENABLED_SHADER_DS_FALSE,
			TE_RENDERER_STATE_ENABLED_SHADER_GS_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_NORMAL_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_SPECULAR_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_ROUGHNESS_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_METALLIC_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_AMBIENTOCCLUSION_FALSE,
			TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE_FALSE,
			TE_RENDERER_STATE_ENABLED_DEPTH_FALSE,
			TE_RENDERER_STATE_ENABLED_STENCIL_FALSE,
			TE_RENDERER_STATE_FILL_MODE_SOLID,
			TE_RENDERER_STATE_CULL_MODE_BACK,
			TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			TE_RENDERER_STATE_COMPARISSON_FUNC_LESS,
			TE_RENDERER_STATE_DEPTH_WRITE_MASK_ZERO
		);
		_RendererStates |= RendererLayer::GetSharedRendererStates();

		Shader* shader = nullptr;
		auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::DEFERREDSHADING, TE_IDX_MESH_TYPE::MESH_POINT, _RendererStates, "Assets/Shaders/DeferredShadingPBR.hlsl", "vs", "ps");

		TE_RESOURCE_FORMAT _RTVFormat[] = { m_RendererLayer->GetFormatRenderTargetScene() };

		PipelineGraphics::Factory(&m_Pipeline, _RendererStates, shader, _countof(_RTVFormat), _RTVFormat, m_RendererLayer->GetFormatDepthStencilScene(), false);
	}
	void RenderPass_DeferredShading::RegisterEventListeners()
	{
	}
	void RenderPass_DeferredShading::UnRegisterEventListeners()
	{
	}
}