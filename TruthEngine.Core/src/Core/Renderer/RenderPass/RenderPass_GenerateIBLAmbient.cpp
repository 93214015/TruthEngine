#include "pch.h"
#include "RenderPass_GenerateIBLAmbient.h"

#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/ShaderManager.h"

namespace TruthEngine
{

	RenderPass_GenerateIBLAmbient::RenderPass_GenerateIBLAmbient(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATEIBLAMBIENT, _RendererLayer)
		, m_Viewport(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f)
		, m_ViewRect(0l, 0l, 0l, 0l)
	{
	}
	void RenderPass_GenerateIBLAmbient::OnAttach()
	{
		TE_ASSERT_CORE(m_IsInitialized, "The RenderPass_GenerateIBLAmbient is not initialized!");

		m_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATEIBLAMBIENT, TE_IDX_SHADERCLASS::GENERATEIBLAMBIENT);

		InitTextures();
		InitBuffers();

		PreparePipline();
	}
	void RenderPass_GenerateIBLAmbient::OnDetach()
	{
		m_RendererCommand.ReleaseResource(m_TextureRenderTargetIBL);

		m_RendererCommand.Release();
	}
	void RenderPass_GenerateIBLAmbient::OnImGuiRender()
	{
	}
	void RenderPass_GenerateIBLAmbient::OnUpdate(double _DeltaTime)
	{
	}
	void RenderPass_GenerateIBLAmbient::BeginScene()
	{
		m_RendererCommand.BeginGraphics(&m_PipelineGenerateIBL);

		m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);
		m_RendererCommand.SetRenderTarget(m_RenderTartgetViewIBL);
		m_RendererCommand.ClearRenderTarget(m_RenderTartgetViewIBL);
	}
	void RenderPass_GenerateIBLAmbient::EndScene()
	{
		m_RendererCommand.End();

		if (m_OutputTextureFilePath.c_str() != "")
			m_RendererCommand.SaveTextureToFile(*m_TextureRenderTargetIBL, m_OutputTextureFilePath.c_str());
	}
	void RenderPass_GenerateIBLAmbient::Render()
	{
		m_RendererCommand.ExecutePendingCommands();

		m_RendererCommand.Draw(1, 0);
	}
	void RenderPass_GenerateIBLAmbient::Initialize(TE_IDX_GRESOURCES _TextureIBLIDX, size_t _TextureIBLSize, TE_RESOURCE_FORMAT _TextureIBLFormat, const char* _OutputFilePath)
	{
		m_TextureIBLIDX = _TextureIBLIDX;
		m_TextureIBLSize = _TextureIBLSize;
		m_TextureIBLFormat = _TextureIBLFormat;
		m_OutputTextureFilePath = _OutputFilePath;

		m_Viewport.Resize(static_cast<float>(_TextureIBLSize), static_cast<float>(_TextureIBLSize));
		m_ViewRect = ViewRect{ 0l, 0l, static_cast<long>(_TextureIBLSize) , static_cast<long>(_TextureIBLSize) };

		m_IsInitialized = true;
	}
	void RenderPass_GenerateIBLAmbient::PreparePipline()
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
		auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::GENERATEIBLAMBIENT, TE_IDX_MESH_TYPE::MESH_POINT, _RendererStates, "Assets/Shaders/GenerateIBLAmbient.hlsl", "vs", "ps", "", "", "", "gs");

		TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT };

		PipelineGraphics::Factory(&m_PipelineGenerateIBL, _RendererStates, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);
	}
	void RenderPass_GenerateIBLAmbient::InitTextures()
	{
		m_TextureRenderTargetIBL = m_RendererCommand.CreateRenderTargetCubeMap(m_TextureIBLIDX, static_cast<uint32_t>(m_TextureIBLSize), static_cast<uint32_t>(m_TextureIBLSize), 1, m_TextureIBLFormat, ClearValue_RenderTarget{ 0.0f, 0.0f, 0.0f, 0.0f }, true, false);

		m_RendererCommand.CreateRenderTargetView(m_TextureRenderTargetIBL, &m_RenderTartgetViewIBL);

	}
	void RenderPass_GenerateIBLAmbient::InitBuffers()
	{
	}
}