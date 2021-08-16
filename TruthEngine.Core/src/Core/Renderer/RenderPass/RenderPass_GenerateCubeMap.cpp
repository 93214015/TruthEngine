#include "pch.h"
#include "RenderPass_GenerateCubeMap.h"

#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/ShaderManager.h"

namespace TruthEngine
{

	constexpr long CUBE_MAP_SIZE = 1024l;

	RenderPass_GenerateCubeMap::RenderPass_GenerateCubeMap(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATECUBEMAP, _RendererLayer)
		, m_Viewport(0.0f, 0.0f, static_cast<float>(CUBE_MAP_SIZE), static_cast<float>(CUBE_MAP_SIZE), 0.0f, 1.0f)
		, m_ViewRect(0l, 0l, CUBE_MAP_SIZE, CUBE_MAP_SIZE)
		, m_TextureIBL(TE_IDX_GRESOURCES::Texture_IBL, 1024, 1024, 1, TE_RESOURCE_FORMAT::UNKNOWN, TE_RESOURCE_USAGE_SHADERRESOURCE, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::COPY_DEST, false)
	{
	}

	void RenderPass_GenerateCubeMap::OnAttach()
	{
		m_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATECUBEMAP, TE_IDX_SHADERCLASS::GENERATECUBEMAP);

		InitTextures();
		InitBuffers();

		PreparePipline();
	}

	void RenderPass_GenerateCubeMap::OnDetach()
	{
		m_RendererCommand.ReleaseResource(m_TextureRenderTargetCubeMap);

		m_RendererCommand.Release();
	}

	void RenderPass_GenerateCubeMap::OnImGuiRender()
	{
	}

	void RenderPass_GenerateCubeMap::OnUpdate(double _DeltaTime)
	{
	}

	void RenderPass_GenerateCubeMap::BeginScene()
	{
		m_RendererCommand.BeginGraphics(&m_Pipeline);

		m_RendererCommand.SetRenderTarget(m_RenderTartgetView);
		m_RendererCommand.ClearRenderTarget(m_RenderTartgetView);
		m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);
	}

	void RenderPass_GenerateCubeMap::EndScene()
	{
		m_RendererCommand.End();
	}

	void RenderPass_GenerateCubeMap::Render()
	{
		m_RendererCommand.ExecutePendingCommands();

		m_RendererCommand.Draw(1, 0);

	}

	void RenderPass_GenerateCubeMap::PreparePipline()
	{
		std::string shaderName = std::string("GenerateCubeMap");

		Shader* shader = nullptr;

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

		auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::GENERATECUBEMAP, TE_IDX_MESH_TYPE::MESH_POINT, _RendererStates, "Assets/Shaders/GenerateCubeMap.hlsl", "vs", "ps", "", "", "", "gs");

		TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT };

		PipelineGraphics::Factory(&m_Pipeline, _RendererStates, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);
	}

	void RenderPass_GenerateCubeMap::InitTextures()
	{
		m_TextureRenderTargetCubeMap = m_RendererCommand.CreateRenderTargetCubeMap(TE_IDX_GRESOURCES::Texture_RT_CubeMap, static_cast<uint32_t>(CUBE_MAP_SIZE), static_cast<uint32_t>(CUBE_MAP_SIZE), TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, ClearValue_RenderTarget{ 0.0f, 0.0f, 0.0f, 0.0f }, true, false);

		m_RendererCommand.CreateRenderTargetView(m_TextureRenderTargetCubeMap, &m_RenderTartgetView);

		m_RendererCommand.LoadTexture(m_TextureIBL, TE_IDX_GRESOURCES::Texture_IBL, "E:\\3DModels\\2021\\Textures\\IBL\\Road_to_MonumentValley.jpg");
	}

	void RenderPass_GenerateCubeMap::InitBuffers()
	{
	}

}