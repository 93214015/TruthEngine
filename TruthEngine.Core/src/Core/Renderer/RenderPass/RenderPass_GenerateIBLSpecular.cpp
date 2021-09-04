#include "pch.h"
#include "RenderPass_GenerateIBLSpecular.h"

#include "Core/Renderer/ShaderManager.h"

namespace TruthEngine
{
	RenderPass_GenerateIBLSpecular::RenderPass_GenerateIBLSpecular(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATEIBLSPECULAR, _RendererLayer)
		, m_Viewport{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }
		, m_ViewRect{ 0l,0l,0l,0l }
	{
	}
	void RenderPass_GenerateIBLSpecular::OnImGuiRender()
	{
	}
	void RenderPass_GenerateIBLSpecular::OnUpdate(double _DeltaTime)
	{
	}
	void RenderPass_GenerateIBLSpecular::BeginScene()
	{
		std::for_each(
			m_ContainerRendererCommand_Prefilter.begin(),
			m_ContainerRendererCommand_Prefilter.end(),
			[this, _IndexRendererCommand = 0](RendererCommand& _RendererCommand) mutable
		{
			_RendererCommand.BeginGraphics(&m_PipelinePrefilter);

			_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);

			_RendererCommand.SetRenderTarget(m_ContainerRTVPrefilter[_IndexRendererCommand]);
			_RendererCommand.ClearRenderTarget(m_ContainerRTVPrefilter[_IndexRendererCommand]);

			_IndexRendererCommand++;
		}
		);

		//BRDF

		m_RendererCommand_BRDF.BeginGraphics(&m_PipelineBRDF);

		Viewport _ViewportBRDF{ 0.0f, 0.0f, static_cast<float>(m_TextureIBLPrecomputeBRDFSize), static_cast<float>(m_TextureIBLPrecomputeBRDFSize) , 0.0f, 1.0f };
		ViewRect _ViewRectBRDF{ 0l, 0l, static_cast<long>(m_TextureIBLPrecomputeBRDFSize), static_cast<long>(m_TextureIBLPrecomputeBRDFSize) };

		m_RendererCommand_BRDF.SetViewPort(&_ViewportBRDF, &_ViewRectBRDF);

		m_RendererCommand_BRDF.SetRenderTarget(m_RTVBRDF);
		m_RendererCommand_BRDF.ClearRenderTarget(m_RTVBRDF);

	}
	void RenderPass_GenerateIBLSpecular::EndScene()
	{
		std::for_each(
			m_ContainerRendererCommand_Prefilter.begin(),
			m_ContainerRendererCommand_Prefilter.end(),
			[this](RendererCommand& _RendererCommand) mutable
			{
				_RendererCommand.End();
			}
		);

		m_RendererCommand_BRDF.End();
	}
	void RenderPass_GenerateIBLSpecular::Render()
	{

		auto _CBData = m_ConstantBufferDirect->GetData();

		std::for_each(
			m_ContainerRendererCommand_Prefilter.begin(),
			m_ContainerRendererCommand_Prefilter.end(),
			[this, _CBData, _MipLevel = 0.0f](RendererCommand& _RendererCommand) mutable
		{
			_RendererCommand.ExecutePendingCommands();

			_CBData->Roughness = _MipLevel / static_cast<float>(m_MipMapLevels - 1);
			_RendererCommand.SetDirectConstantGraphics(m_ConstantBufferDirect);

			_RendererCommand.Draw(1, 0);

			_MipLevel++;
		}
		);


		m_RendererCommand_BRDF.ExecutePendingCommands();
		m_RendererCommand_BRDF.Draw(4, 0);

	}
	void RenderPass_GenerateIBLSpecular::Initialize(size_t _TextureIBLPrefilterEnvironmentSize, size_t _TextureIBLPrecomputeBRDFSize, size_t _MipLevels, TE_RESOURCE_FORMAT _TextureIBLFormat)
	{
		m_TextureIBLPrefilterEnvironmentSize = _TextureIBLPrefilterEnvironmentSize;
		m_TextureIBLPrecomputeBRDFSize = _TextureIBLPrecomputeBRDFSize;
		m_MipMapLevels = _MipLevels;
		m_TextureIBLFormat = _TextureIBLFormat;

		m_Viewport.Resize(static_cast<float>(_TextureIBLPrefilterEnvironmentSize), static_cast<float>(_TextureIBLPrefilterEnvironmentSize));
		m_ViewRect = ViewRect{ 0l, 0l, static_cast<long>(_TextureIBLPrefilterEnvironmentSize) , static_cast<long>(_TextureIBLPrefilterEnvironmentSize) };

		m_ContainerRendererCommand_Prefilter.resize(_MipLevels);
		m_ContainerRTVPrefilter.resize(_MipLevels);

		m_IsInitialized = true;
	}
	void TruthEngine::RenderPass_GenerateIBLSpecular::InitRendererCommand()
	{
		TE_ASSERT_CORE(m_IsInitialized, "The RenderPass_GenerateIBLSpecular is not initialized!");

		std::for_each(
			m_ContainerRendererCommand_Prefilter.begin(),
			m_ContainerRendererCommand_Prefilter.end(),
			[](RendererCommand& _RendererCommand)
			{
				_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATEIBLSPECULAR, TE_IDX_SHADERCLASS::GENERATEIBLSPECULAR_PREFILTER);
			}
		);

		m_RendererCommand_BRDF.Init(TE_IDX_RENDERPASS::GENERATEIBLSPECULAR, TE_IDX_SHADERCLASS::GENERATEIBLSPECULAR_BRDF);
	}
	void RenderPass_GenerateIBLSpecular::InitTextures()
	{
		RendererCommand& _RendererCommand = m_ContainerRendererCommand_Prefilter[0];

		m_RenderTargetIBLSpecularPrefilter = _RendererCommand.CreateRenderTargetCubeMap(TE_IDX_GRESOURCES::Texture_RT_IBL_Specular_Prefilter, m_TextureIBLPrefilterEnvironmentSize, m_TextureIBLPrefilterEnvironmentSize, m_MipMapLevels, m_TextureIBLFormat, ClearValue_RenderTarget{ .0f, .0f, .0f, .0f }, true, false);

		for (size_t i = 0; i < m_MipMapLevels; ++i)
		{
			_RendererCommand.CreateRenderTargetView(m_RenderTargetIBLSpecularPrefilter, &m_ContainerRTVPrefilter[i], i, 0);
		}

		m_RenderTargetIBLSpecularBRDF = _RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_IBL_Specular_BRDF, m_TextureIBLPrecomputeBRDFSize, m_TextureIBLPrecomputeBRDFSize, 1, TE_RESOURCE_FORMAT::R16G16_FLOAT, ClearValue_RenderTarget{ 0.0f, 0.0f, 0.0f, 1.0f }, true, false);

		_RendererCommand.CreateRenderTargetView(m_RenderTargetIBLSpecularBRDF, &m_RTVBRDF);

	}
	void RenderPass_GenerateIBLSpecular::InitBuffers()
	{
		m_ConstantBufferDirect = m_ContainerRendererCommand_Prefilter[0].CreateConstantBufferDirect<ConstantBuffer_Data>(TE_IDX_GRESOURCES::Constant_IBL_Specular);
	}
	void TruthEngine::RenderPass_GenerateIBLSpecular::InitPipelines()
	{
		// Prefilter Environment Map
		{
			constexpr RendererStateSet _RendererStatesPrefilter = InitRenderStates(
				TE_RENDERER_STATE_ENABLED_SHADER_HS_FALSE,
				TE_RENDERER_STATE_ENABLED_SHADER_DS_FALSE,
				TE_RENDERER_STATE_ENABLED_SHADER_GS_TRUE,
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
				TE_RENDERER_STATE_CULL_MODE_NONE,
				TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_POINTLIST,
				TE_RENDERER_STATE_COMPARISSON_FUNC_LESS,
				TE_RENDERER_STATE_DEPTH_WRITE_MASK_ZERO
			);

			Shader* shader = nullptr;
			//Generate Cube Map Shader
			auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::GENERATEIBLSPECULAR_PREFILTER, TE_IDX_MESH_TYPE::MESH_POINT, _RendererStatesPrefilter, "Assets/Shaders/GenerateIBLSpecular_PrefilterEnvironment.hlsl", "vs", "ps", "", "", "", "gs");

			TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT };

			PipelineGraphics::Factory(&m_PipelinePrefilter, _RendererStatesPrefilter, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);
		}

		//Precompute BRDF

		{
			constexpr RendererStateSet _RendererStatesBRDF = InitRenderStates(
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
				TE_RENDERER_STATE_CULL_MODE_NONE,
				TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
				TE_RENDERER_STATE_COMPARISSON_FUNC_LESS,
				TE_RENDERER_STATE_DEPTH_WRITE_MASK_ZERO
			);

			Shader* shader = nullptr;
			auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::GENERATEIBLSPECULAR_BRDF, TE_IDX_MESH_TYPE::MESH_SIMPLE, _RendererStatesBRDF, "Assets/Shaders/GenerateIBLSpecular_PrecomputeBRDF.hlsl", "vs", "ps");

			TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R16G16_FLOAT };

			PipelineGraphics::Factory(&m_PipelineBRDF, _RendererStatesBRDF, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);
		}
	}
	void TruthEngine::RenderPass_GenerateIBLSpecular::ReleaseRendererCommand()
	{
		std::for_each(
			m_ContainerRendererCommand_Prefilter.begin(),
			m_ContainerRendererCommand_Prefilter.end(),
			[](RendererCommand& _RendererCommand)
			{
				_RendererCommand.Release();
			}
		);

		m_RendererCommand_BRDF.Release();

		m_IsInitialized = false;
	}
	void TruthEngine::RenderPass_GenerateIBLSpecular::ReleaseTextures()
	{
	}
	void TruthEngine::RenderPass_GenerateIBLSpecular::ReleaseBuffers()
	{
	}
	void TruthEngine::RenderPass_GenerateIBLSpecular::ReleasePipelines()
	{
	}
	void TruthEngine::RenderPass_GenerateIBLSpecular::RegisterEventListeners()
	{
	}
	void TruthEngine::RenderPass_GenerateIBLSpecular::UnRegisterEventListeners()
	{
	}
	
}