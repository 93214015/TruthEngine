#include "pch.h"
#include "RenderPass_AmbientReflection.h"

#include "Core/Renderer/RendererLayer.h"
#include "Core/Event/EventRenderer.h"

namespace TruthEngine
{
	RenderPass_AmbientReflection::RenderPass_AmbientReflection(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::AMBIENTREFLECTION, _RendererLayer)
	{
	}
	void RenderPass_AmbientReflection::OnImGuiRender()
	{
	}
	void RenderPass_AmbientReflection::OnUpdate(double _DeltaTime)
	{
	}
	void RenderPass_AmbientReflection::BeginScene()
	{
		/////////////////////////
		// Copy HDR Scene Buffer
		/////////////////////////
		{
			m_RendererCommand_CopyResource.BeginGraphics();
			m_RendererCommand_CopyResource.CopyResource(TE_IDX_GRESOURCES::Texture_RT_SceneBufferHDR, m_Texture_SceneBuffer);
		}

		/////////////////////////
		// Blur Reflection
		/////////////////////////
		{
			m_RendererCommand_BlurHorz.BeginCompute(&m_Pipeline_BlurHorz);
			m_RendererCommand_BlurVert.BeginCompute(&m_Pipeline_BlurVert);
		}

		/////////////////////////
		// Render Ambient Reflection
		/////////////////////////
		{
			m_RendererCommand_Blend.BeginGraphics(&m_Pipeline_Blend);
			m_RendererCommand_Blend.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
			m_RendererCommand_Blend.SetRenderTarget(m_RendererLayer->GetRenderTargetViewSceneHDR());
		}
	}
	void RenderPass_AmbientReflection::EndScene()
	{
		m_RendererCommand_CopyResource.End();
		m_RendererCommand_BlurHorz.End();
		m_RendererCommand_BlurVert.End();
		m_RendererCommand_Blend.End();
	}
	void RenderPass_AmbientReflection::Render()
	{

		auto _CBData = m_ConstantBuffer_Blur->GetData();
		_CBData->InputResolution = uint2{ m_Texture_SceneBuffer->GetWidth(), m_Texture_SceneBuffer->GetHeight() };

		//	Horizontally
		{
			auto _ThreadsX = static_cast<uint32_t>(ceilf((float)m_Texture_SceneBuffer->GetWidth() / 64.0f));

			m_RendererCommand_BlurHorz.ExecutePendingCommands();
			m_RendererCommand_BlurHorz.SetDirectConstantCompute(m_ConstantBuffer_Blur);
			m_RendererCommand_BlurHorz.Dispatch(_ThreadsX, m_Texture_SceneBuffer->GetHeight(), 1);
		}
		//	Vertically
		{
			auto _ThreadsY = static_cast<uint32_t>(ceilf((float)m_Texture_SceneBuffer->GetHeight() / 64.0f));

			m_RendererCommand_BlurVert.ExecutePendingCommands();
			m_RendererCommand_BlurVert.SetDirectConstantCompute(m_ConstantBuffer_Blur);
			m_RendererCommand_BlurVert.Dispatch(m_Texture_SceneBuffer->GetWidth(), _ThreadsY, 1);
		}


		{
			m_RendererCommand_Blend.ExecutePendingCommands();
			m_RendererCommand_Blend.Draw(4, 0);
		}

	}
	void RenderPass_AmbientReflection::InitRendererCommand()
	{
		m_RendererCommand_CopyResource.Init(TE_IDX_RENDERPASS::AMBIENTREFLECTION, TE_IDX_SHADERCLASS::NONE);
		m_RendererCommand_BlurHorz.Init(TE_IDX_RENDERPASS::AMBIENTREFLECTION, TE_IDX_SHADERCLASS::BLURHORZREFLECTION);
		m_RendererCommand_BlurVert.Init(TE_IDX_RENDERPASS::AMBIENTREFLECTION, TE_IDX_SHADERCLASS::BLURVERTREFLECTION);
		m_RendererCommand_Blend.Init(TE_IDX_RENDERPASS::AMBIENTREFLECTION, TE_IDX_SHADERCLASS::BLENDREFLECTION);
	}
	void RenderPass_AmbientReflection::InitTextures()
	{
		auto _Width = TE_INSTANCE_APPLICATION->GetSceneViewportWidth();
		_Width = _Width != 0 ? _Width : 1;
		auto _Height = TE_INSTANCE_APPLICATION->GetSceneViewportHeight();
		_Height = _Height != 0 ? _Height : 1;

		m_Texture_SceneBuffer = m_RendererCommand_BlurHorz.CreateTexture(TE_IDX_GRESOURCES::Texture_InputAmbientReflection_SceneBuffer, _Width, _Height, 1, 1, m_RendererLayer->GetFormatRenderTargetSceneHDR(), TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::COPY_DEST, nullptr);
		m_Texture_SceneBufferBlur = m_RendererCommand_BlurHorz.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_SSReflectionBlur, _Width, _Height, m_RendererLayer->GetFormatRenderTargetSceneHDR(), true, false);
		m_Texture_SceneBufferBlur_Temp = m_RendererCommand_BlurHorz.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_SSReflectionBlur_Temp, _Width, _Height, m_RendererLayer->GetFormatRenderTargetSceneHDR(), true, false);
	}
	void RenderPass_AmbientReflection::InitBuffers()
	{
		m_ConstantBuffer_Blur = m_RendererCommand_BlurHorz.CreateConstantBufferDirect<ConstantBufferData_Blur>(TE_IDX_GRESOURCES::Constant_ReflectionBlur);
	}
	void RenderPass_AmbientReflection::InitPipelines()
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

			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::BLENDREFLECTION, 0, "Assets/Shaders/AmbientReflection.hlsl", "vs", "ps");

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
			const auto _ShaderHandle_BlurHorz = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::BLURHORZREFLECTION, 0, "Assets/Shaders/CSGaussianBlur.hlsl", "", "", "HorizontalFilter", "", "", "", { L"KernelHalf=15" });
			PipelineCompute::Factory(&m_Pipeline_BlurHorz, _ShaderHandle_BlurHorz);

			const auto _ShaderHandle_BlurVert = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::BLURVERTREFLECTION, 0, "Assets/Shaders/CSGaussianBlur.hlsl", "", "", "VerticalFilter", "", "", "", { L"KernelHalf=15" });
			PipelineCompute::Factory(&m_Pipeline_BlurVert, _ShaderHandle_BlurVert);
		}
	}
	void RenderPass_AmbientReflection::ReleaseRendererCommand()
	{
		m_RendererCommand_CopyResource.Release();
		m_RendererCommand_Blend.Release();
		m_RendererCommand_BlurHorz.Release();
		m_RendererCommand_BlurVert.Release();

	}
	void RenderPass_AmbientReflection::ReleaseTextures()
	{
		m_RendererCommand_BlurHorz.ReleaseResource(m_Texture_SceneBuffer);
		m_RendererCommand_BlurHorz.ReleaseResource(m_Texture_SceneBufferBlur);
		m_RendererCommand_BlurHorz.ReleaseResource(m_Texture_SceneBufferBlur_Temp);
	}
	void RenderPass_AmbientReflection::ReleaseBuffers()
	{
	}
	void RenderPass_AmbientReflection::ReleasePipelines()
	{
	}
	void RenderPass_AmbientReflection::RegisterEventListeners()
	{
		auto _Lambda_OnEventRendererViewportResize = [this](Event& _Event)
		{
			OnEventRendererViewportResize(static_cast<EventRendererViewportResize&>(_Event));
		};

		m_EventListenerList.push_back(TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, _Lambda_OnEventRendererViewportResize));
	}
	void RenderPass_AmbientReflection::UnRegisterEventListeners()
	{
		TE_INSTANCE_APPLICATION->UnRegisterEventListener(m_EventListenerList.data(), m_EventListenerList.size());
	}
	void RenderPass_AmbientReflection::OnEventRendererViewportResize(EventRendererViewportResize& _Event)
	{
		InitTextures();
	}
}