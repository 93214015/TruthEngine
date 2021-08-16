#include "pch.h"
#include "RenderPass_GenerateCubeMap.h"

#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/ShaderManager.h"

namespace TruthEngine
{



	RenderPass_GenerateCubeMap::RenderPass_GenerateCubeMap(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATECUBEMAP, _RendererLayer)
		, m_Viewport(0.0f, 0.0f, 1024.0f, 1024.0f, 0.0f, 1.0f)
		, m_ViewRect(0l, 0l, 1024l, 1024l)
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
	}

	void RenderPass_GenerateCubeMap::OnImGuiRender()
	{
	}

	void RenderPass_GenerateCubeMap::OnUpdate(double _DeltaTime)
	{
	}

	void RenderPass_GenerateCubeMap::BeginScene()
	{
	}

	void RenderPass_GenerateCubeMap::EndScene()
	{
	}

	void RenderPass_GenerateCubeMap::Render()
	{
	}

	void RenderPass_GenerateCubeMap::PreparePipline()
	{
		std::string shaderName = std::string("GenerateCubeMap");

		Shader* shader = nullptr;

		RendererStateSet states = InitRenderStates();
		SET_RENDERER_STATE(states, TE_RENDERER_STATE_CULL_MODE, TE_RENDERER_STATE_CULL_MODE::TE_RENDERER_STATE_CULL_MODE_NONE);
		SET_RENDERER_STATE(states, TE_RENDERER_STATE_DEPTH_WRITE_MASK, TE_RENDERER_STATE_DEPTH_WRITE_MASK::TE_RENDERER_STATE_DEPTH_WRITE_MASK_ZERO);

		auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::GENERATECUBEMAP, TE_IDX_MESH_TYPE::MESH_POINT, states, "Assets/Shaders/GenerateCubeMap.hlsl", "vs", "ps", "", "", "", "gs");

		TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT };

		PipelineGraphics::Factory(&m_Pipeline, states, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);
	}

	void RenderPass_GenerateCubeMap::InitTextures()
	{
	}

	void RenderPass_GenerateCubeMap::InitBuffers()
	{
	}

}