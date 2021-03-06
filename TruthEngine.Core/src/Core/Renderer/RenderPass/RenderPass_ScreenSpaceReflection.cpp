#include "pch.h"
#include "RenderPass_ScreenSpaceReflection.h"

#include "Core/Renderer/RendererLayer.h"

#include "Core/Event/EventRenderer.h"

#include "Core/Profiler/GPUEvents.h"

namespace TruthEngine
{
	RenderPass_ScreenSpaceReflection::RenderPass_ScreenSpaceReflection(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::SSREFLECTION, _RendererLayer)
	{
	}
	void RenderPass_ScreenSpaceReflection::OnImGuiRender()
	{
		if (ImGui::Begin("Screen Space Reflection"))
		{
			static float _MaxDistance = m_ConstantBuffer_Reflection->GetData()->MaxDistance;
			if (ImGui::DragFloat("Ray Max Distance", &_MaxDistance, 0.1f, 0.0f, 100.0f))
			{
				m_RendererCommand_Reflection.AddUpdateTask([=]() 
					{
						m_ConstantBuffer_Reflection->GetData()->MaxDistance = _MaxDistance;
					}
				);
			}
			static float _Resolution = m_ConstantBuffer_Reflection->GetData()->Resolution;
			if (ImGui::DragFloat("Resolution", &_Resolution, 0.1f, 0.0f, 1.0f))
			{
				m_RendererCommand_Reflection.AddUpdateTask([=]()
					{
						m_ConstantBuffer_Reflection->GetData()->Resolution = _Resolution;
					}
				);
			}
			static float _Thickness = m_ConstantBuffer_Reflection->GetData()->Thickness;
			if (ImGui::DragFloat("Thickness", &_Thickness, 0.1f, 0.0f, 100.0f))
			{
				m_RendererCommand_Reflection.AddUpdateTask([=]()
					{
						m_ConstantBuffer_Reflection->GetData()->Thickness = _Thickness;
					}
				);
			}
			static float _Steps = m_ConstantBuffer_Reflection->GetData()->Steps;
			if (ImGui::DragFloat("Resolve Steps", &_Steps, 1.0f, 1.0f, 100.0f, "%.1f"))
			{
				m_RendererCommand_Reflection.AddUpdateTask([=]()
					{
						m_ConstantBuffer_Reflection->GetData()->Steps = _Steps;
					}
				);
			}
			static float _ViewAngleThreshold = m_ConstantBuffer_Reflection->GetData()->ViewAngleThreshold;
			if (ImGui::DragFloat("View Angle Threshold", &_ViewAngleThreshold, 0.1f, 0.1f, 1.0f, "%.1f"))
			{
				m_RendererCommand_Reflection.AddUpdateTask([=]()
					{
						m_ConstantBuffer_Reflection->GetData()->ViewAngleThreshold = _ViewAngleThreshold;
					}
				);
			}
			static float _ReflectionScale = m_ConstantBuffer_Reflection->GetData()->ReflectionScale;
			if (ImGui::DragFloat("Reflection Scale", &_ReflectionScale, 0.1f, 0.1f, 1.0f, "%.1f"))
			{
				m_RendererCommand_Reflection.AddUpdateTask([=]()
					{
						m_ConstantBuffer_Reflection->GetData()->ReflectionScale = _ReflectionScale;
					}
				);
			}
			static float _DepthBias = m_ConstantBuffer_Reflection->GetData()->DepthBias;
			if (ImGui::DragFloat("Depth Bias", &_DepthBias, 0.001f, 0.0f, 5.0f, "%.3f"))
			{
				m_RendererCommand_Reflection.AddUpdateTask([=]()
					{
						m_ConstantBuffer_Reflection->GetData()->DepthBias = _DepthBias;
					}
				);
			}
		}
		ImGui::End();
	}
	void RenderPass_ScreenSpaceReflection::OnUpdate(double _DeltaTime)
	{
	}
	void RenderPass_ScreenSpaceReflection::BeginScene()
	{
		/////////////////////////
		// Generate Reflection
		/////////////////////////
		{
			m_RendererCommand_Reflection.BeginGraphics(&m_Pipeline_Reflection);
			TE_GPUBEGINEVENT(m_RendererCommand_Reflection, "ScreenSpaceReflection");
			m_RendererCommand_Reflection.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
			m_RendererCommand_Reflection.SetRenderTarget(m_RTV_Reflection);
			m_RendererCommand_Reflection.ClearRenderTarget(m_RTV_Reflection);
		}

		/////////////////////////
		// Blend Reflection
		/////////////////////////
		/*{
			m_RendererCommand_Blend.BeginGraphics(&m_Pipeline_Blend);
			m_RendererCommand_Blend.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
			m_RendererCommand_Blend.SetRenderTarget(m_RendererLayer->GetRenderTargetViewSceneHDR());
		}*/

		/////////////////////////
		// Blur Reflection
		/////////////////////////
		/*{
			m_RendererCommand_BlurHorz.BeginCompute(&m_Pipeline_BlurHorz);
			m_RendererCommand_BlurVert.BeginCompute(&m_Pipeline_BlurVert);
		}*/
	}
	void RenderPass_ScreenSpaceReflection::EndScene()
	{
		TE_GPUENDEVENT(m_RendererCommand_Reflection);
		m_RendererCommand_Reflection.End();
		//m_RendererCommand_Blend.End();
		//m_RendererCommand_BlurHorz.End();
		//m_RendererCommand_BlurVert.End();
	}
	void RenderPass_ScreenSpaceReflection::Render()
	{
		/////////////////////////
		// Generate Reflection
		/////////////////////////
		{
			m_RendererCommand_Reflection.ExecutePendingCommands();
			m_RendererCommand_Reflection.Draw(4, 0);
		}

		/////////////////////////
		// Blend Reflection
		/////////////////////////
		/*{
			m_RendererCommand_Blend.ExecutePendingCommands();
			m_RendererCommand_Blend.Draw(4, 0);
		}*/

		/////////////////////////
		// Blur Reflection
		/////////////////////////

		/*
		auto _CBData = m_ConstantBuffer_Blur->GetData();
		_CBData->InputResolution = uint2{ m_Texture_ReflectionBlur->GetWidth(), m_Texture_ReflectionBlur->GetHeight() };

		//	Horizontally
		{
			auto _ThreadsX = static_cast<uint32_t>(ceilf((float)m_Texture_ReflectionBlur->GetWidth() / 64.0f));

			m_RendererCommand_BlurHorz.ExecutePendingCommands();
			m_RendererCommand_BlurHorz.SetDirectConstantCompute(m_ConstantBuffer_Blur);
			m_RendererCommand_BlurHorz.Dispatch(_ThreadsX, m_Texture_ReflectionBlur->GetHeight(), 1);
		}
		//	Vertically
		{
			auto _ThreadsY = static_cast<uint32_t>(ceilf((float)m_Texture_ReflectionBlur->GetHeight() / 64.0f));

			m_RendererCommand_BlurVert.ExecutePendingCommands();
			m_RendererCommand_BlurVert.SetDirectConstantCompute(m_ConstantBuffer_Blur);
			m_RendererCommand_BlurVert.Dispatch(m_Texture_ReflectionBlur->GetWidth(), _ThreadsY, 1);
		}
		*/

	}
	void RenderPass_ScreenSpaceReflection::InitRendererCommand()
	{
		m_RendererCommand_Reflection.Init(TE_IDX_RENDERPASS::SSREFLECTION, TE_IDX_SHADERCLASS::SSREFLECTION);
		//m_RendererCommand_Blend.Init(TE_IDX_RENDERPASS::SSREFLECTION, TE_IDX_SHADERCLASS::BLENDREFLECTION);
		//m_RendererCommand_BlurHorz.Init(TE_IDX_RENDERPASS::SSREFLECTION, TE_IDX_SHADERCLASS::BLURHORZREFLECTION);
		//m_RendererCommand_BlurVert.Init(TE_IDX_RENDERPASS::SSREFLECTION, TE_IDX_SHADERCLASS::BLURVERTREFLECTION);
	}
	void RenderPass_ScreenSpaceReflection::InitTextures()
	{
		auto _Width = TE_INSTANCE_APPLICATION->GetSceneViewportWidth();
		_Width = _Width != 0 ? _Width : 1;
		auto _Height = TE_INSTANCE_APPLICATION->GetSceneViewportHeight();
		_Height = _Height != 0 ? _Height : 1;

		//m_RenderTarget_Reflection = m_RendererCommand_Reflection.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SSReflection, _Width, _Height, 1, m_RendererLayer->GetFormatRenderTargetSceneHDR(), ClearValue_RenderTarget{ 0.0f, 0.0f, 0.0f, 0.0f }, true, false);
		m_RenderTarget_Reflection = m_RendererCommand_Reflection.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SSReflection, _Width, _Height, 1, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, ClearValue_RenderTarget{ 0.0f, 0.0f, 0.0f, 0.0f }, true, false);
		//m_Texture_ReflectionBlur = m_RendererCommand_Reflection.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_SSReflectionBlur, _Width, _Height, m_RendererLayer->GetFormatRenderTargetSceneHDR(), true, false);
		//m_Texture_ReflectionBlur_Temp = m_RendererCommand_Reflection.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_SSReflectionBlur_Temp, _Width, _Height, m_RendererLayer->GetFormatRenderTargetSceneHDR(), true, false);


		m_RendererCommand_Reflection.CreateRenderTargetView(m_RenderTarget_Reflection, &m_RTV_Reflection);

	}
	void RenderPass_ScreenSpaceReflection::InitBuffers()
	{
		m_ConstantBuffer_Reflection = m_RendererCommand_Reflection.CreateConstantBufferUpload<ConstantBufferData_SSReflection>(TE_IDX_GRESOURCES::CBuffer_SSReflection);

		//m_ConstantBuffer_Blur = m_RendererCommand_Reflection.CreateConstantBufferDirect<ConstantBufferData_Blur>(TE_IDX_GRESOURCES::Constant_ReflectionBlur);
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

			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::SSREFLECTION, 0, "Assets/Shaders/ScreenSpaceReflection_UV.hlsl", "vs", "ps");

			TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT };
			PipelineGraphics::Factory(&m_Pipeline_Reflection, _States_Reflection, _ShaderHandle, _countof(rtvFormats), rtvFormats, m_RendererLayer->GetFormatDepthStencilSceneDSV(), {}, false);
						

		}

		//Init Blending Reflection's Pipeline
		/*
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

			PipelineGraphics::Factory(&m_Pipeline_Blend, _States_Blend, _ShaderHandle, _countof(rtvFormats), rtvFormats, m_RendererLayer->GetFormatDepthStencilSceneDSV(), {}, false, _BlendDesc);
		}

		//Init Blurring Reflection's Pipelines
		{
			const RendererStateSet _States = InitRenderStates();

			const auto _ShaderHandle_BlurHorz = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::BLURHORZREFLECTION, 0, "Assets/Shaders/CSGaussianBlur.hlsl", "", "", "HorizontalFilter", "", "", "", { L"KernelHalf=6" });
			PipelineCompute::Factory(&m_Pipeline_BlurHorz, _ShaderHandle_BlurHorz);

			const auto _ShaderHandle_BlurVert = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::BLURVERTREFLECTION, 0, "Assets/Shaders/CSGaussianBlur.hlsl", "", "", "VerticalFilter", "", "", "", { L"KernelHalf=6" });
			PipelineCompute::Factory(&m_Pipeline_BlurVert, _ShaderHandle_BlurVert);
		}
		*/

	}
	void RenderPass_ScreenSpaceReflection::ReleaseRendererCommand()
	{
		m_RendererCommand_Reflection.Release();
		//m_RendererCommand_Blend.Release();
		//m_RendererCommand_BlurHorz.Release();
		//m_RendererCommand_BlurVert.Release();
	}
	void RenderPass_ScreenSpaceReflection::ReleaseTextures()
	{
		m_RendererCommand_Reflection.ReleaseResource(m_RenderTarget_Reflection);
		//m_RendererCommand_Reflection.ReleaseResource(m_Texture_ReflectionBlur);
		//m_RendererCommand_Reflection.ReleaseResource(m_Texture_ReflectionBlur_Temp);
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