#include "pch.h"
#include "RenderPass_EnvironmentMap.h"

#include "Core/Renderer/RendererLayer.h"
#include "Core/Profiler/GPUEvents.h"
#include "Core/Entity/Scene.h"
#include "Core/Entity/Components/EnvironmentComponent.h"

namespace TruthEngine
{
	RenderPass_EnvironmentMap::RenderPass_EnvironmentMap(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::EnvironmentMap, _RendererLayer)
	{}
	void RenderPass_EnvironmentMap::OnImGuiRender()
	{
	}
	void RenderPass_EnvironmentMap::OnUpdate(double _DeltaTime)
	{
	}
	void RenderPass_EnvironmentMap::BeginScene()
	{
		m_RendererCommand.BeginGraphics(&m_PipelineEnvironmentMap);
		TE_GPUBEGINEVENT(m_RendererCommand, "EnvironmetMap");

		m_RendererCommand.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());

		m_RTV = &(Settings::Graphics::IsEnabledMSAA() ? (Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetRenderTargetViewSceneHDRMS() : m_RendererLayer->GetRenderTargetViewSceneSDRMS()) : (Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetRenderTargetViewSceneHDR() : m_RendererLayer->GetRenderTargetViewSceneSDR()));
		m_DSV = &(Settings::Graphics::IsEnabledMSAA() ? m_RendererLayer->GetDepthStencilViewSceneMS() : m_RendererLayer->GetDepthStencilViewScene());
		
		m_RendererCommand.SetRenderTarget(*m_RTV);
		m_RendererCommand.SetDepthStencil(*m_DSV);
	}
	void RenderPass_EnvironmentMap::EndScene()
	{
		TE_GPUENDEVENT(m_RendererCommand);
		m_RendererCommand.End();
	}
	void RenderPass_EnvironmentMap::Render()
	{
		m_RendererCommand.ExecutePendingCommands();

		Scene* _ActiveScene = GetActiveScene();
		const auto _EntityViewEnv = _ActiveScene->ViewEntities<EnvironmentComponent>();

		m_RendererCommand.SetDirectConstantGraphics(m_ConstantBufferDirect_EnvironmentMap);

		for (auto& _EntityEnv : _EntityViewEnv)
		{
			const Mesh* mesh = &_ActiveScene->GetComponent<EnvironmentComponent>(_EntityEnv).GetMesh();
			m_RendererCommand.DrawIndexed(mesh);
		}
	}
	void RenderPass_EnvironmentMap::InitRendererCommand()
	{
		m_RendererCommand.Init(TE_IDX_RENDERPASS::EnvironmentMap, TE_IDX_SHADERCLASS::RENDERENVIRONMENTMAP);
	}
	void RenderPass_EnvironmentMap::InitTextures()
	{
		m_RTV = &(Settings::Graphics::IsEnabledMSAA() ? (Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetRenderTargetViewSceneHDRMS() : m_RendererLayer->GetRenderTargetViewSceneSDRMS()) : (Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetRenderTargetViewSceneHDR() : m_RendererLayer->GetRenderTargetViewSceneSDR()));
		m_DSV = &(Settings::Graphics::IsEnabledMSAA() ? m_RendererLayer->GetDepthStencilViewSceneMS() : m_RendererLayer->GetDepthStencilViewScene());
	}
	void RenderPass_EnvironmentMap::InitBuffers()
	{
		m_ConstantBufferDirect_EnvironmentMap = static_cast<ConstantBufferDirect<ConstantBuffer_Data_EnvironmentMap>*>(TE_INSTANCE_BUFFERMANAGER->GetConstantBufferDirect(TE_IDX_GRESOURCES::Constant_EnvironmentMap));
	}
	void RenderPass_EnvironmentMap::InitPipelines()
	{
		RendererStateSet states = InitRenderStates();
		SET_RENDERER_STATE(states, TE_RENDERER_STATE_CULL_MODE, TE_RENDERER_STATE_CULL_MODE::TE_RENDERER_STATE_CULL_MODE_NONE);

		const TE_RESOURCE_FORMAT rtvFormats[] = { Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetFormatRenderTargetSceneHDR() : m_RendererLayer->GetFormatRenderTargetSceneSDR() };

		const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::RENDERENVIRONMENTMAP, 0, "Assets/Shaders/RenderEnvironmentCube.hlsl", "vs", "ps");

		const PipelineDepthStencilDesc _PipelineDSDesc{ TE_DEPTH_WRITE_MASK::ZERO, TE_COMPARISON_FUNC::LESS_EQUAL };

		const std::vector<ShaderInputElement> _InputElements{ {"POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0 } };

		PipelineGraphics::Factory(&m_PipelineEnvironmentMap, states, _ShaderHandle, 1, rtvFormats, m_RendererLayer->GetFormatDepthStencilSceneDSV(), _InputElements, true, PipelineBlendDesc{}, _PipelineDSDesc);
	}
	void RenderPass_EnvironmentMap::ReleaseRendererCommand()
	{
		m_RendererCommand.Release();
	}
	void RenderPass_EnvironmentMap::ReleaseTextures()
	{
	}
	void RenderPass_EnvironmentMap::ReleaseBuffers()
	{
	}
	void RenderPass_EnvironmentMap::ReleasePipelines()
	{
	}
	void RenderPass_EnvironmentMap::RegisterEventListeners()
	{
	}
	void RenderPass_EnvironmentMap::UnRegisterEventListeners()
	{
	}
}