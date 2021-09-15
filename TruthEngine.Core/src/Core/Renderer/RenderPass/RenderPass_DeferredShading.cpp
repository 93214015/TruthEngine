#include "pch.h"
#include "RenderPass_DeferredShading.h"

#include "Core/Application.h"
#include "Core/Event/Event.h"

#include "Core/Renderer/ShaderManager.h"
#include "Core/Renderer/RendererLayer.h"

namespace TruthEngine
{
	enum DeferredShading_Shader_SettingMask : uint64_t
	{
		DeferredShading_SettingMask_HDR = 1 << 0,
	};

	static std::pair<uint64_t, std::vector<const wchar_t*>>  GenerateShaderID()
	{
		std::pair<uint64_t, std::vector<const wchar_t*>> _Pair;

		if (Settings::Graphics::IsEnabledHDR())
		{
			_Pair.first |= DeferredShading_SettingMask_HDR;
			_Pair.second.emplace_back(L"ENABLE_HDR");
		}

		return _Pair;
	}

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
		m_RendererCommand.SetRenderTarget(Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetRenderTargetViewSceneHDR() : m_RendererLayer->GetRenderTargetViewSceneSDR());
		
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
			TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		);
		_RendererStates |= RendererLayer::GetSharedRendererStates();
		
		auto [_UniqueID, _Macros] = GenerateShaderID();

		auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::DEFERREDSHADING, _UniqueID, "Assets/Shaders/DeferredShadingPBR.hlsl", "vs", "ps", "", "", "", "", _Macros);

		TE_RESOURCE_FORMAT _RTVFormat[] = { Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetFormatRenderTargetSceneHDR() : m_RendererLayer->GetFormatRenderTargetSceneSDR() };

		PipelineDepthStencilDesc _PipelineDSDesc{ TE_DEPTH_WRITE_MASK::ZERO, TE_COMPARISON_FUNC::LESS };

		PipelineGraphics::Factory(&m_Pipeline, _RendererStates, _ShaderHandle, _countof(_RTVFormat), _RTVFormat, TE_RESOURCE_FORMAT::UNKNOWN, {}, false, PipelineBlendDesc{}, _PipelineDSDesc);
	}
	void RenderPass_DeferredShading::RegisterEventListeners()
	{
	}
	void RenderPass_DeferredShading::UnRegisterEventListeners()
	{
	}
}