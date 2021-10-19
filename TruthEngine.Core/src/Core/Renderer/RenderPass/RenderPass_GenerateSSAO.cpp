#include "pch.h"
#include "RenderPass_GenerateSSAO.h"

#include "Core/Renderer/ShaderManager.h"
#include "Core/Renderer/RendererLayer.h"

#include "Core/Profiler/GPUEvents.h"

#include <random>

namespace TruthEngine
{

	static constexpr uint32_t MedianBlurSize = 2;
	static constexpr uint32_t MedianBlurMaxBinSize = 100;

	RenderPass_GenerateSSAO::RenderPass_GenerateSSAO(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATESSAO, _RendererLayer)
	{
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
		std::default_random_engine generator;

		//Generate Kernel Samples
		for (unsigned int i = 0; i < _countof(m_KernelSamples); ++i)
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
		for (unsigned int i = 0; i < _countof(m_Noises); i++)
		{
			m_Noises[i] = float3(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f);
		}
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
		TE_GPUBEGINEVENT(m_RendererCommand, "SSAO");
		m_RendererCommand.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
		m_RendererCommand.SetRenderTarget(m_RTV);
		m_RendererCommand.ClearRenderTarget(m_RTV);


		m_RendererCommand_BlurringCS.BeginCompute(&m_PipelineBlurringCS);
		TE_GPUBEGINEVENT(m_RendererCommand_BlurringCS, "SSAO->Blurring BoxFilter CS");

		/*
		m_RendererCommand_Blurring.BeginGraphics(&m_PipelineBlurring);
		TE_GPUBEGINEVENT(m_RendererCommand_Blurring, "SSAO->Blurring BoxFilter");
		m_RendererCommand_Blurring.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
		m_RendererCommand_Blurring.SetRenderTarget(m_RTVBlurred);
		m_RendererCommand_Blurring.ClearRenderTarget(m_RTVBlurred);

		m_RendererCommand_MedianBlurring.BeginGraphics(&m_PipelineBlurringMedian);
		TE_GPUBEGINEVENT(m_RendererCommand_MedianBlurring, "SSAO->Blurring Median");
		m_RendererCommand_MedianBlurring.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
		m_RendererCommand_MedianBlurring.SetRenderTarget(m_RTVBlurredMedian);

		m_RendererCommand_MedianBlurringCS.BeginCompute(&m_PipelineBlurringMedianCS);
		TE_GPUBEGINEVENT(m_RendererCommand_MedianBlurringCS, "SSAO->Blurring Median CS");
		*/

	}

	void RenderPass_GenerateSSAO::EndScene()
	{
		TE_GPUENDEVENT(m_RendererCommand);
		m_RendererCommand.End();

		TE_GPUENDEVENT(m_RendererCommand_BlurringCS);
		m_RendererCommand_BlurringCS.End();

		/*
		TE_GPUENDEVENT(m_RendererCommand_Blurring);
		m_RendererCommand_Blurring.End();

		TE_GPUENDEVENT(m_RendererCommand_MedianBlurring);
		m_RendererCommand_MedianBlurring.End();

		TE_GPUENDEVENT(m_RendererCommand_MedianBlurringCS);
		m_RendererCommand_MedianBlurringCS.End();
		*/
	}

	void RenderPass_GenerateSSAO::Render()
	{
		/////////////////////////////////////////////////////
		//Generate Screeen Space Ambient Occlusion
		/////////////////////////////////////////////////////
		m_RendererCommand.ExecutePendingCommands();

		m_RendererCommand.Draw(4, 0);


		/////////////////////////////////////////////////////
		// Blurring SSAO by Box Filtering Compute Shader
		/////////////////////////////////////////////////////
		m_RendererCommand_BlurringCS.Dispatch(m_MedianBlurringDispatchThreadNum.x, m_MedianBlurringDispatchThreadNum.y, 1);


		/*
		/////////////////////////////////////////////////////
		// Blurring SSAO by Box Filtering
		/////////////////////////////////////////////////////
		m_RendererCommand_Blurring.ExecutePendingCommands();

		m_RendererCommand_Blurring.Draw(4, 0);


		/////////////////////////////////////////////////////
		// Blurring SSAO by Median Filtering
		/////////////////////////////////////////////////////
		m_RendererCommand_MedianBlurring.ExecutePendingCommands();

		m_RendererCommand_MedianBlurring.Draw(4, 0);

		/////////////////////////////////////////////////////
		// Blurring SSAO by Median Filtering Compute Shader
		/////////////////////////////////////////////////////
		m_RendererCommand_MedianBlurringCS.Dispatch(m_MedianBlurringDispatchThreadNum.x, m_MedianBlurringDispatchThreadNum.y, 1);
		*/
	}

	void RenderPass_GenerateSSAO::InitRendererCommand()
	{
		m_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATESSAO, TE_IDX_SHADERCLASS::GENERATESSAO);
		m_RendererCommand_BlurringCS.Init(TE_IDX_RENDERPASS::GENERATESSAO, TE_IDX_SHADERCLASS::SSAOBLURRINGCS);
		/*
		m_RendererCommand_Blurring.Init(TE_IDX_RENDERPASS::GENERATESSAO, TE_IDX_SHADERCLASS::SSAOBLURRING);
		m_RendererCommand_MedianBlurring.Init(TE_IDX_RENDERPASS::GENERATESSAO, TE_IDX_SHADERCLASS::SSAOBLURRINGMEDIAN);
		m_RendererCommand_MedianBlurringCS.Init(TE_IDX_RENDERPASS::GENERATESSAO, TE_IDX_SHADERCLASS::SSAOBLURRINGMEDIANCS);
		*/
	}

	void RenderPass_GenerateSSAO::InitTextures()
	{
		Application* _App = TE_INSTANCE_APPLICATION;
		m_TextureRenderTargetSSAO = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SSAO, _App->GetSceneViewportWidth(), _App->GetSceneViewportHeight(), 1, TE_RESOURCE_FORMAT::R32_FLOAT, ClearValue_RenderTarget{ 1.0f, 1.0f, 1.0f, 1.0f }, true, false);

		//m_TextureRenderTargetSSAOBlurred = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SSAOBlurred, _App->GetSceneViewportWidth(), _App->GetSceneViewportHeight(), 1, TE_RESOURCE_FORMAT::R32_FLOAT, ClearValue_RenderTarget{ 1.0f, 1.0f, 1.0f, 1.0f }, true, false);
		//m_TextureRenderTargetSSAOBlurredMedian = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SSAOBlurredMedian, _App->GetSceneViewportWidth(), _App->GetSceneViewportHeight(), 1, TE_RESOURCE_FORMAT::R32_FLOAT, ClearValue_RenderTarget{ 1.0f, 1.0f, 1.0f, 1.0f }, true, false);

		m_RendererCommand.CreateRenderTargetView(m_TextureRenderTargetSSAO, &m_RTV);
		//m_RendererCommand.CreateRenderTargetView(m_TextureRenderTargetSSAOBlurred, &m_RTVBlurred);
		//m_RendererCommand.CreateRenderTargetView(m_TextureRenderTargetSSAOBlurredMedian, &m_RTVBlurredMedian);
		

		m_TextureNoises = m_RendererCommand.CreateTexture(TE_IDX_GRESOURCES::Texture_SSAONoises, 4, 4, 1, 1, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::COPY_DEST, m_Noises);

		m_TextureRWBlurred = m_RendererCommand.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_SSAOBlurred, _App->GetSceneViewportWidth(), _App->GetSceneViewportHeight(), TE_RESOURCE_FORMAT::R32_FLOAT, true, false);
		//m_TextureRWBlurredMedian = m_RendererCommand.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_SSAOBlurredMedian, _App->GetSceneViewportWidth(), _App->GetSceneViewportHeight(), TE_RESOURCE_FORMAT::R32_FLOAT, true, false);

		SetThreadNum(_App->GetSceneViewportWidth(), _App->GetSceneViewportHeight());
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

		m_ConstantBufferBlurringCS = m_RendererCommand_BlurringCS.CreateConstantBufferUpload<ConstantBufferData_SSAOBlurringCS>(TE_IDX_GRESOURCES::CBuffer_SSAO_BlurringCS);

		m_RendererCommand_BlurringCS.AddUpdateTask([this]() 
			{
				Application* _App = TE_INSTANCE_APPLICATION;
				auto _CBData = m_ConstantBufferBlurringCS->GetData();
				_CBData->TextureSize = uint2(_App->GetSceneViewportWidth(), _App->GetSceneViewportHeight());
			}
		);

	}
	void RenderPass_GenerateSSAO::InitPipelines()
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

		TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R32_FLOAT };

		///////////////////////////////////////////////
		//// SSAO Generation
		///////////////////////////////////////////////
		{
			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::GENERATESSAO, 0, "Assets/Shaders/GenerateSSAO.hlsl", "vs", "ps");

			PipelineGraphics::Factory(&m_Pipeline, _RendererStates, _ShaderHandle, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::UNKNOWN, {}, false);
		}


		///////////////////////////////////////////////
		//// SSAO BoxFilter Compute Shader
		///////////////////////////////////////////////
		{
			const std::wstring _KernelHalfSize = L"SIZE=" + std::to_wstring(MedianBlurSize);
			const std::wstring _TextureType = L"TEX_TYPE=float";

			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::SSAOBLURRINGCS, 0, "Assets/Shaders/CSBlurBox.hlsl", "", "", "cs", "", "", "", { _TextureType.c_str(), _KernelHalfSize.c_str()});

			PipelineCompute::Factory(&m_PipelineBlurringCS, _ShaderHandle);
		}

		/*
		
		///////////////////////////////////////////////
		//// SSAO Blurring
		///////////////////////////////////////////////
		{
			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::SSAOBLURRING, 0, "Assets/Shaders/SSAOBlur.hlsl", "vs", "ps");

			PipelineGraphics::Factory(&m_PipelineBlurring, _RendererStates, _ShaderHandle, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::UNKNOWN, {}, false);
		}

		///////////////////////////////////////////////
		//// SSAO Median Blurring Graphics Shader
		///////////////////////////////////////////////
		{
			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::SSAOBLURRINGMEDIAN, 0, "Assets/Shaders/BlurMedian.hlsl", "vs", "ps", "", "", "", "", {L"TEX_TYPE=float"});

			PipelineGraphics::Factory(&m_PipelineBlurringMedian, _RendererStates, _ShaderHandle, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::UNKNOWN, {}, false);
		}

		///////////////////////////////////////////////
		//// SSAO MedianFilter Compute Shader
		///////////////////////////////////////////////
		{
			const std::wstring _TextureType = L"TEX_TYPE=float";
			const std::wstring _KernelHalfSize = L"SIZE=" + std::to_wstring(MedianBlurSize);
			const std::wstring _MaxBinSize = L"MAX_BIN_SIZE=" + std::to_wstring(MedianBlurMaxBinSize);

			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::SSAOBLURRINGMEDIANCS, 0, "Assets/Shaders/CSBlurMedian.hlsl", "", "", "cs", "", "", "", { _TextureType.c_str(), _KernelHalfSize.c_str(), _MaxBinSize.c_str()});

			PipelineCompute::Factory(&m_PipelineBlurringMedianCS, _ShaderHandle);
		}

		*/
	}

	void RenderPass_GenerateSSAO::ReleaseRendererCommand()
	{
		m_RendererCommand.Release();
		m_RendererCommand_BlurringCS.Release();
		/*
		m_RendererCommand_Blurring.Release();
		m_RendererCommand_MedianBlurring.Release();
		m_RendererCommand_MedianBlurringCS.Release();
		*/
	}

	void RenderPass_GenerateSSAO::ReleaseTextures()
	{
		m_RendererCommand.ReleaseResource(m_TextureRenderTargetSSAO);
		m_RendererCommand.ReleaseResource(m_TextureRWBlurred);
		/*
		m_RendererCommand.ReleaseResource(m_TextureRenderTargetSSAOBlurred);
		m_RendererCommand.ReleaseResource(m_TextureNoises);
		m_RendererCommand.ReleaseResource(m_TextureRenderTargetSSAOBlurredMedian);
		m_RendererCommand.ReleaseResource(m_TextureRWBlurredMedian);
		*/
	}

	void RenderPass_GenerateSSAO::ReleaseBuffers()
	{
		m_RendererCommand.ReleaseResource(m_ConstantBufferSSAO);
		m_RendererCommand.ReleaseResource(m_ConstantBufferBlurringCS);
	}

	void RenderPass_GenerateSSAO::ReleasePipelines()
	{
		
	}

	void RenderPass_GenerateSSAO::RegisterEventListeners()
	{
		m_EventListenerList.push_back(TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, [this](Event& _Event)
			{
				OnEventRendererViewportResize(static_cast<EventRendererViewportResize&>(_Event));
			}
		));
	}

	void RenderPass_GenerateSSAO::UnRegisterEventListeners()
	{
		TE_INSTANCE_APPLICATION->UnRegisterEventListener(m_EventListenerList.data(), m_EventListenerList.size());
	}

	void RenderPass_GenerateSSAO::SetThreadNum(uint32_t _Width, uint32_t _Height)
	{
		uint32_t _ThreadNumX = static_cast<uint32_t>(ceilf(static_cast<float>(_Width) / static_cast<float>(MedianBlurSize * 2.0f + 1.0f)));
		uint32_t _ThreadNumY = static_cast<uint32_t>(ceilf(static_cast<float>(_Height) / static_cast<float>(MedianBlurSize * 2.0f + 1.0f)));

		m_MedianBlurringDispatchThreadNum = uint2{ _ThreadNumX, _ThreadNumY };
	}


	void RenderPass_GenerateSSAO::OnEventRendererViewportResize(const EventRendererViewportResize& _Event)
	{
		m_RendererCommand.ResizeRenderTarget(m_TextureRenderTargetSSAO, _Event.GetWidth(), _Event.GetHeight(), &m_RTV, nullptr);
		//m_RendererCommand.ResizeRenderTarget(m_TextureRenderTargetSSAOBlurred, _Event.GetWidth(), _Event.GetHeight(), &m_RTVBlurred, nullptr);
		//m_RendererCommand.ResizeRenderTarget(m_TextureRenderTargetSSAOBlurredMedian, _Event.GetWidth(), _Event.GetHeight(), &m_RTVBlurredMedian, nullptr);

		//m_TextureRWBlurredMedian = m_RendererCommand_MedianBlurring.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_SSAOBlurredMedian, _Event.GetWidth(), _Event.GetHeight(), TE_RESOURCE_FORMAT::R32_FLOAT, true, false);
		m_TextureRWBlurred = m_RendererCommand_BlurringCS.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_SSAOBlurred, _Event.GetWidth(), _Event.GetHeight(), TE_RESOURCE_FORMAT::R32_FLOAT, true, false);

		m_RendererCommand_BlurringCS.AddUpdateTask([this]()
			{
				Application* _App = TE_INSTANCE_APPLICATION;
				auto _CBData = m_ConstantBufferBlurringCS->GetData();
				_CBData->TextureSize = uint2(_App->GetSceneViewportWidth(), _App->GetSceneViewportHeight());
			}
		);

		SetThreadNum(_Event.GetWidth(), _Event.GetHeight());
	}

}