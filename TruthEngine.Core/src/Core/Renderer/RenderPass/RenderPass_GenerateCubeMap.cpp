#include "pch.h"
#include "RenderPass_GenerateCubeMap.h"

#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/ShaderManager.h"

namespace TruthEngine
{


	RenderPass_GenerateCubeMap::RenderPass_GenerateCubeMap(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATECUBEMAP, _RendererLayer)
		, m_Viewport(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f)
		, m_ViewRect(0l, 0l, 0l, 0l)
		, m_TextureEnvironment(TE_IDX_GRESOURCES::Texture_InputCreateCubeMap, 0, 0, 1, 1, TE_RESOURCE_FORMAT::UNKNOWN, TE_RESOURCE_USAGE_SHADERRESOURCE, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::COPY_DEST, false)
	{
	}

	void RenderPass_GenerateCubeMap::OnAttach()
	{
		TE_ASSERT_CORE(m_IsInitialized, "The resources for RenderPass_GenerateCubeMap are not set");

		m_RendererCommand_GenerateCubeMap.Init(TE_IDX_RENDERPASS::GENERATECUBEMAP, TE_IDX_SHADERCLASS::GENERATECUBEMAP);
		//m_RendererCommand_GenerateIBL.Init(TE_IDX_RENDERPASS::GENERATECUBEMAP, TE_IDX_SHADERCLASS::GENERATEIBL);

		InitTextures();
		InitBuffers();

		PreparePipline();
	}

	void RenderPass_GenerateCubeMap::OnDetach()
	{
		m_RendererCommand_GenerateCubeMap.ReleaseResource(&m_TextureEnvironment);

		m_RendererCommand_GenerateCubeMap.Release();
		//m_RendererCommand_GenerateIBL.Release();

		m_IsInitialized = false;
	}

	void RenderPass_GenerateCubeMap::OnImGuiRender()
	{
	}

	void RenderPass_GenerateCubeMap::OnUpdate(double _DeltaTime)
	{
	}

	void RenderPass_GenerateCubeMap::BeginScene()
	{

		m_RendererCommand_GenerateCubeMap.BeginGraphics(&m_PipelineGenerateCubeMap);

		m_RendererCommand_GenerateCubeMap.SetRenderTarget(m_RenderTartgetViewCubeMap);
		m_RendererCommand_GenerateCubeMap.ClearRenderTarget(m_RenderTartgetViewCubeMap);
		m_RendererCommand_GenerateCubeMap.SetViewPort(&m_Viewport, &m_ViewRect);

		//m_RendererCommand_GenerateIBL.BeginGraphics(&m_PipelineGenerateIBL);

		//m_RendererCommand_GenerateIBL.SetRenderTarget(m_RenderTartgetViewIBL);
		//m_RendererCommand_GenerateIBL.ClearRenderTarget(m_RenderTartgetViewIBL);
		//m_RendererCommand_GenerateIBL.SetViewPort(&m_Viewport, &m_ViewRect);
	}

	void RenderPass_GenerateCubeMap::EndScene()
	{
		m_RendererCommand_GenerateCubeMap.End();
		//m_RendererCommand_GenerateIBL.End();
	}

	void RenderPass_GenerateCubeMap::Render()
	{

		m_RendererCommand_GenerateCubeMap.ExecutePendingCommands();

		m_RendererCommand_GenerateCubeMap.Draw(1, 0);

		//m_RendererCommand_GenerateIBL.ExecutePendingCommands();

		//m_RendererCommand_GenerateIBL.Draw(1, 0);
	}

	void RenderPass_GenerateCubeMap::Initialize(TE_IDX_GRESOURCES _TextureCubeIDX, size_t _CubeMapSize, TE_RESOURCE_FORMAT _TextureCubeFormat, const char* _FilePath)
	{
		m_TextureCubeIDX = _TextureCubeIDX;
		m_CubeMapSize = _CubeMapSize;
		m_TextureCubeFormat = _TextureCubeFormat;
		m_InputTextureFilePath = _FilePath;

		m_Viewport.Resize(static_cast<float>(_CubeMapSize), static_cast<float>(_CubeMapSize));
		m_ViewRect = ViewRect{ 0, 0, static_cast<long>(_CubeMapSize), static_cast<long>(_CubeMapSize) };

		m_IsInitialized = true;
	}

	void RenderPass_GenerateCubeMap::PreparePipline()
	{
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

		//Generate Cube Map Shader
		auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::GENERATECUBEMAP, TE_IDX_MESH_TYPE::MESH_POINT, _RendererStates, "Assets/Shaders/GenerateCubeMap.hlsl", "vs", "ps", "", "", "", "gs");

		TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT };

		PipelineGraphics::Factory(&m_PipelineGenerateCubeMap, _RendererStates, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);

		//Generate IBL Shader
		//result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::GENERATEIBL, TE_IDX_MESH_TYPE::MESH_POINT, _RendererStates, "Assets/Shaders/GenerateIBL.hlsl", "vs", "ps", "", "", "", "gs");

		//PipelineGraphics::Factory(&m_PipelineGenerateIBL, _RendererStates, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);
	}

	void RenderPass_GenerateCubeMap::InitTextures()
	{
		m_TextureRenderTargetCubeMap = m_RendererCommand_GenerateCubeMap.CreateRenderTargetCubeMap(m_TextureCubeIDX, static_cast<uint32_t>(m_CubeMapSize), static_cast<uint32_t>(m_CubeMapSize), 1, m_TextureCubeFormat, ClearValue_RenderTarget{ 0.0f, 0.0f, 0.0f, 0.0f }, true, false);
		//m_TextureRenderTargetIBL = m_RendererCommand_GenerateCubeMap.CreateRenderTargetCubeMap(TE_IDX_GRESOURCES::Texture_RT_IBL, static_cast<uint32_t>(CUBE_MAP_SIZE), static_cast<uint32_t>(CUBE_MAP_SIZE), TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, ClearValue_RenderTarget{ 0.0f, 0.0f, 0.0f, 0.0f }, true, false);

		m_RendererCommand_GenerateCubeMap.CreateRenderTargetView(m_TextureRenderTargetCubeMap, &m_RenderTartgetViewCubeMap);
		//m_RendererCommand_GenerateCubeMap.CreateRenderTargetView(m_TextureRenderTargetIBL, &m_RenderTartgetViewIBL);

		//m_RendererCommand_GenerateCubeMap.LoadTextureFromFile(m_TextureEnvironment, TE_IDX_GRESOURCES::Texture_Environment, "E:\\3DModels\\2021\\Textures\\IBL\\Road_to_MonumentValley_8k.jpg");
		m_RendererCommand_GenerateCubeMap.LoadTextureFromFile(m_TextureEnvironment, TE_IDX_GRESOURCES::Texture_InputCreateCubeMap, m_InputTextureFilePath.c_str());
	}

	void RenderPass_GenerateCubeMap::InitBuffers()
	{
	}

}