#include "pch.h"
#include "RenderPass_GenerateSSAO.h"

#include "Core/Renderer/ShaderManager.h"
#include "Core/Renderer/RendererLayer.h"

#include <random>

namespace TruthEngine
{



	RenderPass_GenerateSSAO::RenderPass_GenerateSSAO(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATESSAO, _RendererLayer)
	{}

	void RenderPass_GenerateSSAO::OnAttach()
	{
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
		std::default_random_engine generator;

		//Generate Kernel Samples
		for (unsigned int i = 0; i < 64; ++i)
		{
			m_KernelSamples[i] = float3A(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator)
			);
			Math::Normalize(m_KernelSamples[i]);
			m_KernelSamples[i] *= randomFloats(generator);
		}

		//Generate Noises
		for (unsigned int i = 0; i < 16; i++)
		{
			m_Noises[i] = float3A(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f);
		}

		m_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATESSAO, TE_IDX_SHADERCLASS::GENERATESSAO);

		m_RendererCommand_Blurring.Init(TE_IDX_RENDERPASS::GENERATESSAO, TE_IDX_SHADERCLASS::SSAOBLURRING);

		InitTextures();
		InitBuffers();
		InitPipeline();

		RegisterEvents();
	}

	void RenderPass_GenerateSSAO::OnDetach()
	{

		ReleaseTextures();
		ReleaseBuffers();
		ReleasePipeline();

		m_RendererCommand.Release();
	}

	void RenderPass_GenerateSSAO::OnImGuiRender()
	{
		static float s_SSAORadius = m_ConstantBufferSSAO->GetData()->SSAORadius;
		static float s_SSAODepthBias = m_ConstantBufferSSAO->GetData()->SSAODepthBias;

		if (ImGui::Begin("SSAO"))
		{

			if (ImGui::DragFloat("SSAO Radius", &s_SSAORadius, 0.01, 0.0f))
			{
				m_RendererCommand.AddUpdateTask([this]() 
					{
						m_ConstantBufferSSAO->GetData()->SSAORadius = s_SSAORadius;
					}
				);
			}

			if (ImGui::DragFloat("SSAO DepthBias", &s_SSAODepthBias, 0.001, 0.0f))
			{
				m_RendererCommand.AddUpdateTask([this]()
					{
						m_ConstantBufferSSAO->GetData()->SSAODepthBias = s_SSAODepthBias;
					}
				);
			}


		}
		ImGui::End();
	}

	void RenderPass_GenerateSSAO::OnUpdate(double _DeltaTime)
	{
	}

	void RenderPass_GenerateSSAO::BeginScene()
	{
		m_RendererCommand.BeginGraphics(&m_Pipeline);

		m_RendererCommand.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
		m_RendererCommand.SetRenderTarget(m_RTV);
		m_RendererCommand.ClearRenderTarget(m_RTV);

		m_RendererCommand_Blurring.BeginGraphics(&m_PipelineBlurring);

		m_RendererCommand_Blurring.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
		m_RendererCommand_Blurring.SetRenderTarget(m_RTVBlurred);
		m_RendererCommand_Blurring.ClearRenderTarget(m_RTVBlurred);
	}

	void RenderPass_GenerateSSAO::EndScene()
	{
		m_RendererCommand.End();

		m_RendererCommand_Blurring.End();
	}

	void RenderPass_GenerateSSAO::Render()
	{
		m_RendererCommand.ExecutePendingCommands();

		m_RendererCommand.Draw(4, 0);


		m_RendererCommand_Blurring.ExecutePendingCommands();

		m_RendererCommand_Blurring.Draw(4, 0);
	}

	void RenderPass_GenerateSSAO::InitTextures()
	{
		Application* _App = TE_INSTANCE_APPLICATION;
		m_TextureRenderTargetSSAO = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SSAO, _App->GetSceneViewportWidth(), _App->GetSceneViewportHeight(), 1, TE_RESOURCE_FORMAT::R32_FLOAT, ClearValue_RenderTarget{ 1.0f, 1.0f, 1.0f, 1.0f }, true, false);
		m_TextureRenderTargetSSAOBlurred = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SSAOBlurred, _App->GetSceneViewportWidth(), _App->GetSceneViewportHeight(), 1, TE_RESOURCE_FORMAT::R32_FLOAT, ClearValue_RenderTarget{ 1.0f, 1.0f, 1.0f, 1.0f }, true, false);

		m_RendererCommand.CreateRenderTargetView(m_TextureRenderTargetSSAO, &m_RTV);
		m_RendererCommand.CreateRenderTargetView(m_TextureRenderTargetSSAOBlurred, &m_RTVBlurred);

		m_TextureNoises = m_RendererCommand.CreateTexture(TE_IDX_GRESOURCES::Texture_SSAONoises, 4, 4, 1, 1, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::COPY_DEST, m_Noises);
	}

	void RenderPass_GenerateSSAO::InitBuffers()
	{
		m_ConstantBufferSSAO = m_RendererCommand.CreateConstantBufferUpload<ConstantBufferData_SSAO>(TE_IDX_GRESOURCES::CBuffer_SSAO);


		m_RendererCommand.AddUpdateTask([this]()
		{
			auto _CBData = m_ConstantBufferSSAO->GetData();
			_CBData->SSAORadius = 0.5f;
			_CBData->SSAODepthBias = 0.025f;
			memcpy(_CBData->KernelSamples, m_KernelSamples, 64 * sizeof(float3A));
		});

	}
	void RenderPass_GenerateSSAO::InitPipeline()
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

		TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R32_FLOAT };

		Shader* shader = nullptr;
		auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::GENERATESSAO, TE_IDX_MESH_TYPE::MESH_SIMPLE, _RendererStates, "Assets/Shaders/GenerateSSAO.hlsl", "vs", "ps");

		PipelineGraphics::Factory(&m_Pipeline, _RendererStates, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);



		result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::SSAOBLURRING, TE_IDX_MESH_TYPE::MESH_SIMPLE, _RendererStates, "Assets/Shaders/SSAOBlur.hlsl", "vs", "ps");

		PipelineGraphics::Factory(&m_PipelineBlurring, _RendererStates, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);
	}

	void RenderPass_GenerateSSAO::ReleaseTextures()
	{
		m_RendererCommand.ReleaseResource(m_TextureRenderTargetSSAO);
		m_RendererCommand.ReleaseResource(m_TextureNoises);
	}

	void RenderPass_GenerateSSAO::ReleaseBuffers()
	{
	}

	void RenderPass_GenerateSSAO::ReleasePipeline()
	{
	}

	void RenderPass_GenerateSSAO::RegisterEvents()
	{
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, [this](Event& _Event) 
			{
				OnEventRendererViewportResize(static_cast<EventRendererViewportResize&>(_Event));
			}
		);
	}

	void RenderPass_GenerateSSAO::OnEventRendererViewportResize(const EventRendererViewportResize& _Event)
	{
		m_RendererCommand.ResizeRenderTarget(m_TextureRenderTargetSSAO, _Event.GetWidth(), _Event.GetHeight(), &m_RTV, nullptr);
		m_RendererCommand.ResizeRenderTarget(m_TextureRenderTargetSSAOBlurred, _Event.GetWidth(), _Event.GetHeight(), &m_RTVBlurred, nullptr);
	}

}