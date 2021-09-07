#include "pch.h"
#include "RenderPass_ScreenSpaceReflection.h"

#include "Core/Renderer/RendererLayer.h"

#include "Core/Event/EventRenderer.h"

namespace TruthEngine
{
	RenderPass_ScreenSpaceReflection::RenderPass_ScreenSpaceReflection(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::SSREFLECTION, _RendererLayer)
	{
	}
	void RenderPass_ScreenSpaceReflection::OnImGuiRender()
	{
	}
	void RenderPass_ScreenSpaceReflection::OnUpdate(double _DeltaTime)
	{
	}
	void RenderPass_ScreenSpaceReflection::BeginScene()
	{
		{
			m_RendererCommand_Reflection.BeginGraphics(&m_Pipeline_Reflection);
			m_RendererCommand_Reflection.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
			m_RendererCommand_Reflection.SetRenderTarget(m_RTV_Reflection);
			m_RendererCommand_Reflection.ClearRenderTarget(m_RTV_Reflection);
		}

		{
			m_RendererCommand_Blend.BeginGraphics(&m_Pipeline_Blend);
			m_RendererCommand_Blend.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
			m_RendererCommand_Blend.SetRenderTarget(m_RendererLayer->GetRenderTargetViewSceneHDR());
		}
	}
	void RenderPass_ScreenSpaceReflection::EndScene()
	{
		m_RendererCommand_Reflection.End();
		m_RendererCommand_Blend.End();
	}
	void RenderPass_ScreenSpaceReflection::Render()
	{
		{
			m_RendererCommand_Reflection.ExecutePendingCommands();
			m_RendererCommand_Reflection.Draw(4, 0);
		}

		{
			m_RendererCommand_Blend.ExecutePendingCommands();
			m_RendererCommand_Blend.Draw(4, 0);
		}

	}
	void RenderPass_ScreenSpaceReflection::InitRendererCommand()
	{
		m_RendererCommand_Reflection.Init(TE_IDX_RENDERPASS::SSREFLECTION, TE_IDX_SHADERCLASS::SSREFLECTION);
		m_RendererCommand_Blend.Init(TE_IDX_RENDERPASS::SSREFLECTION, TE_IDX_SHADERCLASS::BLENDREFLECTION);
	}
	void RenderPass_ScreenSpaceReflection::InitTextures()
	{
		auto _Width = TE_INSTANCE_APPLICATION->GetSceneViewportWidth();
		_Width = _Width != 0 ? _Width : 1;
		auto _Height = TE_INSTANCE_APPLICATION->GetSceneViewportHeight();
		_Height = _Height != 0 ? _Height : 1;

		m_RenderTarget_Reflection = m_RendererCommand_Reflection.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SSReflection, _Width, _Height, 1, m_RendererLayer->GetFormatRenderTargetSceneHDR(), ClearValue_RenderTarget{ 0.0f, 0.0f, 0.0f, 0.0f }, true, false);

		m_RendererCommand_Reflection.CreateRenderTargetView(m_RenderTarget_Reflection, &m_RTV_Reflection);

	}
	void RenderPass_ScreenSpaceReflection::InitBuffers()
	{
		m_ConstantBuffer_Reflection = m_RendererCommand_Reflection.CreateConstantBufferUpload<ConstantBufferData_SSReflection>(TE_IDX_GRESOURCES::CBuffer_SSReflection);

		m_RendererCommand_Reflection.AddUpdateTask([this]() { *m_ConstantBuffer_Reflection->GetData() = ConstantBufferData_SSReflection(0.2f, 0.45f, 0.5f, 1.0f); });
	}
	void RenderPass_ScreenSpaceReflection::InitPipelines()
	{
		//Init Screen Space Reflection Pipeline
		{
			const RendererStateSet _States_Reflection = InitRenderStates
			(
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
				TE_RENDERER_STATE_ENABLED_HDR_FALSE,
				TE_RENDERER_STATE_SHADING_MODEL_NONE,
				TE_RENDERER_STATE_ENABLED_BLEND_FALSE
			);

			Shader* shader = nullptr;
			auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::SSREFLECTION, TE_IDX_MESH_TYPE::MESH_POINT, _States_Reflection, "Assets/Shaders/ScreenSpaceReflection.hlsl", "vs", "ps");

			TE_RESOURCE_FORMAT rtvFormats[] = { m_RendererLayer->GetFormatRenderTargetSceneHDR() };

			PipelineGraphics::Factory(&m_Pipeline_Reflection, _States_Reflection, shader, _countof(rtvFormats), rtvFormats, m_RendererLayer->GetFormatDepthStencilSceneDSV(), false);
		}

		//Init Blending Reflection's Pipeline
		{
			const RendererStateSet _States_Blend = InitRenderStates
			(
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
				TE_RENDERER_STATE_ENABLED_HDR_FALSE,
				TE_RENDERER_STATE_SHADING_MODEL_NONE,
				TE_RENDERER_STATE_ENABLED_BLEND_TRUE
			);

			Shader* shader = nullptr;
			auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::BLENDREFLECTION, TE_IDX_MESH_TYPE::MESH_POINT, _States_Blend, "Assets/Shaders/BlendReflection.hlsl", "vs", "ps");

			TE_RESOURCE_FORMAT rtvFormats[] = { m_RendererLayer->GetFormatRenderTargetSceneHDR() };

			PipelineBlendDesc _BlendDesc
			{
				TE_BLEND::SRC_COLOR,
				TE_BLEND::DEST_COLOR,
				TE_BLEND_OP::ADD,
				TE_BLEND::SRC_ALPHA,
				TE_BLEND::INV_SRC_ALPHA,
				TE_BLEND_OP::ADD,
				TE_COLOR_WRITE_ENABLE::TE_COLOR_WRITE_ENABLE_ALL
			};

			PipelineGraphics::Factory(&m_Pipeline_Blend, _States_Blend, shader, _countof(rtvFormats), rtvFormats, m_RendererLayer->GetFormatDepthStencilSceneDSV(), false, _BlendDesc);
		}
	}
	void RenderPass_ScreenSpaceReflection::ReleaseRendererCommand()
	{
		m_RendererCommand_Reflection.Release();
		m_RendererCommand_Blend.Release();
	}
	void RenderPass_ScreenSpaceReflection::ReleaseTextures()
	{
		m_RendererCommand_Reflection.ReleaseResource(m_RenderTarget_Reflection);
	}
	void RenderPass_ScreenSpaceReflection::ReleaseBuffers()
	{
		m_RendererCommand_Reflection.ReleaseResource(m_ConstantBuffer_Reflection);
	}
	void RenderPass_ScreenSpaceReflection::ReleasePipelines()
	{
	}
	void RenderPass_ScreenSpaceReflection::RegisterEventListeners()
	{
		auto _Lambda_OnEventRendererViewportResize = [this](Event& _Event)
		{
			OnEventRendererViewportResize(static_cast<EventRendererViewportResize&>(_Event));
		};

		m_EventListenerList.push_back(TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, _Lambda_OnEventRendererViewportResize));
	}
	void RenderPass_ScreenSpaceReflection::UnRegisterEventListeners()
	{
		TE_INSTANCE_APPLICATION->UnRegisterEventListener(m_EventListenerList.data(), m_EventListenerList.size());
	}
	void RenderPass_ScreenSpaceReflection::OnEventRendererViewportResize(EventRendererViewportResize& _Event)
	{
		InitTextures();
	}
}