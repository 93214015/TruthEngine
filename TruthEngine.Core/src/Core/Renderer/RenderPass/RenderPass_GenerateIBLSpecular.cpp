#include "pch.h"
#include "RenderPass_GenerateIBLSpecular.h"

#include "Core/Renderer/ShaderManager.h"

namespace TruthEngine
{
	RenderPass_GenerateIBLSpecular::RenderPass_GenerateIBLSpecular(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATEIBLSPECULAR, _RendererLayer)
		, m_Viewport{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
		, m_ViewRect{0l,0l,0l,0l}
	{
	}
	void RenderPass_GenerateIBLSpecular::OnAttach()
	{
		TE_ASSERT_CORE(m_IsInitialized, "The RenderPass_GenerateIBLSpecular is not initialized!");

		m_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATEIBLSPECULAR, TE_IDX_SHADERCLASS::GENERATEIBLSPECULAR);

		InitTextures();
		InitBuffers();
		InitPipeline();
	}
	void RenderPass_GenerateIBLSpecular::OnDetach()
	{
		m_RendererCommand.Release();

		m_IsInitialized = false;
	}
	void RenderPass_GenerateIBLSpecular::OnImGuiRender()
	{
	}
	void RenderPass_GenerateIBLSpecular::OnUpdate(double _DeltaTime)
	{
	}
	void RenderPass_GenerateIBLSpecular::BeginScene()
	{
		m_RendererCommand.BeginGraphics(&m_Pipeline);

		m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);

		m_RendererCommand.SetRender
	}
	void RenderPass_GenerateIBLSpecular::EndScene()
	{
	}
	void RenderPass_GenerateIBLSpecular::Render()
	{
	}
	void RenderPass_GenerateIBLSpecular::Initialize(size_t _TextureIBLSize, size_t _MipLevels, TE_RESOURCE_FORMAT _TextureIBLFormat)
	{
		m_TextureIBLSize = _TextureIBLSize;
		m_MipMapLevels = _MipLevels;
		m_TextureIBLFormat = _TextureIBLFormat;

		m_RTVs.resize(_MipLevels);

		m_IsInitialized = true;
	}
	void RenderPass_GenerateIBLSpecular::InitTextures()
	{
		m_RenderTargetIBLSpecular = m_RendererCommand.CreateRenderTargetCubeMap(TE_IDX_GRESOURCES::Texture_RT_IBL_Specular, m_TextureIBLSize, m_TextureIBLSize, m_MipMapLevels, m_TextureIBLFormat, ClearValue_RenderTarget{ .0f, .0f, .0f, .0f }, true, false);

		for (size_t i = 0; i < m_MipMapLevels; ++i)
		{
			m_RendererCommand.CreateRenderTargetView(m_RenderTargetIBLSpecular, &m_RTVs[i], i, 0);
		}
	}
	void RenderPass_GenerateIBLSpecular::InitBuffers()
	{
	}
	void RenderPass_GenerateIBLSpecular::InitPipeline()
	{
		constexpr RendererStateSet _RendererStates = InitRenderStates(
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
		auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::GENERATEIBLSPECULAR, TE_IDX_MESH_TYPE::MESH_POINT, _RendererStates, "Assets/Shaders/GenerateIBLSpecular.hlsl", "vs", "ps", "", "", "", "gs");

		TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT };

		PipelineGraphics::Factory(&m_Pipeline, _RendererStates, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);
	}
}