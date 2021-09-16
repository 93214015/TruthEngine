#include "pch.h"
#include "RenderPass_BlendReflection.h"

#include "Core/Event/EventRenderer.h"

namespace TruthEngine
{
	void RenderPass_BlendReflection::OnImGuiRender()
	{
	}
	void RenderPass_BlendReflection::OnUpdate(double _DeltaTime)
	{
	}
	void RenderPass_BlendReflection::BeginScene()
	{

		/////////////////////////
		// Blur Reflection
		/////////////////////////
		{
			m_RendererCommand_BlurHorz.BeginCompute(&m_Pipeline_BlurHorz);
			m_RendererCommand_BlurVert.BeginCompute(&m_Pipeline_BlurVert);
		}

		/////////////////////////
		// Blend Reflection
		/////////////////////////
		{
			m_RendererCommand_Blend.BeginGraphics(&m_Pipeline_Blend);
			m_RendererCommand_Blend.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
			m_RendererCommand_Blend.SetRenderTarget(m_RendererLayer->GetRenderTargetViewSceneHDR());
			m_RendererCommand_Blend.Copy();
		}
	}
	void RenderPass_BlendReflection::EndScene()
	{
	}
	void RenderPass_BlendReflection::Render()
	{
	}
	void RenderPass_BlendReflection::InitRendererCommand()
	{
		m_RendererCommand_Blend.Init(TE_IDX_RENDERPASS::SSREFLECTION, TE_IDX_SHADERCLASS::BLENDREFLECTION);
		m_RendererCommand_BlurHorz.Init(TE_IDX_RENDERPASS::SSREFLECTION, TE_IDX_SHADERCLASS::BLURHORZREFLECTION);
		m_RendererCommand_BlurVert.Init(TE_IDX_RENDERPASS::SSREFLECTION, TE_IDX_SHADERCLASS::BLURVERTREFLECTION);
	}
	void RenderPass_BlendReflection::InitTextures()
	{
		auto _Width = TE_INSTANCE_APPLICATION->GetSceneViewportWidth();
		_Width = _Width != 0 ? _Width : 1;
		auto _Height = TE_INSTANCE_APPLICATION->GetSceneViewportHeight();
		_Height = _Height != 0 ? _Height : 1;

		m_Texture_SceneBuffer = m_RendererCommand_BlurHorz.CreateTexture(TE_IDX_GRESOURCES::Texture_InputBlendReflection_SceneBuffer, _Width, _Height, 1, 1, m_RendererLayer->GetFormatRenderTargetSceneHDR(), TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::COPY_DEST, nullptr);
		m_Texture_SceneBufferBlur = m_RendererCommand_BlurHorz.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_SSReflectionBlur, _Width, _Height, m_RendererLayer->GetFormatRenderTargetSceneHDR(), true, false);
		m_Texture_SceneBufferBlur_Temp = m_RendererCommand_BlurHorz.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_SSReflectionBlur_Temp, _Width, _Height, m_RendererLayer->GetFormatRenderTargetSceneHDR(), true, false);
	}
	void RenderPass_BlendReflection::InitBuffers()
	{
		m_ConstantBuffer_Blur = m_RendererCommand_BlurHorz.CreateConstantBufferDirect<ConstantBufferData_Blur>(TE_IDX_GRESOURCES::Constant_ReflectionBlur);
	}
	void RenderPass_BlendReflection::InitPipelines()
	{

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

			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::BLENDREFLECTION, 0, "Assets/Shaders/BlendReflection.hlsl", "vs", "ps");

			TE_RESOURCE_FORMAT rtvFormats[] = { m_RendererLayer->GetFormatRenderTargetSceneHDR() };

			PipelineBlendDesc _BlendDesc
			{
				TE_BLEND::SRC_ALPHA,
				TE_BLEND::ONE,
				TE_BLEND_OP::ADD,
				TE_BLEND::SRC_ALPHA,
				TE_BLEND::INV_SRC_ALPHA,
				TE_BLEND_OP::ADD,
				TE_COLOR_WRITE_ENABLE::TE_COLOR_WRITE_ENABLE_ALL
			};

			PipelineGraphics::Factory(&m_Pipeline_Blend, _States_Blend, _ShaderHandle, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::UNKNOWN, {}, false, _BlendDesc);
		}

		//Init Blurring Reflection's Pipelines
		{
			const auto _ShaderHandle_BlurHorz = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::BLURHORZREFLECTION, 0, "Assets/Shaders/CSGaussianBlur.hlsl", "", "", "HorizontalFilter", "", "", "", { L"KernelHalf=6" });
			PipelineCompute::Factory(&m_Pipeline_BlurHorz, _ShaderHandle_BlurHorz);

			const auto _ShaderHandle_BlurVert = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::BLURVERTREFLECTION, 0, "Assets/Shaders/CSGaussianBlur.hlsl", "", "", "VerticalFilter", "", "", "", { L"KernelHalf=6" });
			PipelineCompute::Factory(&m_Pipeline_BlurVert, _ShaderHandle_BlurVert);
		}
	}
	void RenderPass_BlendReflection::ReleaseRendererCommand()
	{
		m_RendererCommand_Blend.Release();
		m_RendererCommand_BlurHorz.Release();
		m_RendererCommand_BlurVert.Release();

	}
	void RenderPass_BlendReflection::ReleaseTextures()
	{
		m_RendererCommand_BlurHorz.ReleaseResource(m_Texture_SceneBuffer);
		m_RendererCommand_BlurHorz.ReleaseResource(m_Texture_SceneBufferBlur);
		m_RendererCommand_BlurHorz.ReleaseResource(m_Texture_SceneBufferBlur_Temp);
	}
	void RenderPass_BlendReflection::ReleaseBuffers()
	{
	}
	void RenderPass_BlendReflection::ReleasePipelines()
	{
	}
	void RenderPass_BlendReflection::RegisterEventListeners()
	{
		auto _Lambda_OnEventRendererViewportResize = [this](Event& _Event)
		{
			OnEventRendererViewportResize(static_cast<EventRendererViewportResize&>(_Event));
		};

		m_EventListenerList.push_back(TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, _Lambda_OnEventRendererViewportResize));
	}
	void RenderPass_BlendReflection::UnRegisterEventListeners()
	{
		TE_INSTANCE_APPLICATION->UnRegisterEventListener(m_EventListenerList.data(), m_EventListenerList.size());
	}
	void RenderPass_BlendReflection::OnEventRendererViewportResize(EventRendererViewportResize& _Event)
	{
		InitTextures();
	}
}