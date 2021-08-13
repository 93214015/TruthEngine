#include "pch.h"
#include "RenderPass_DeferredShading.h"

#include "Core/Application.h"
#include "Core/Event/Event.h"

#include "Core/Renderer/ShaderManager.h"

namespace TruthEngine
{
	RenderPass_DeferredShading::RenderPass_DeferredShading(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::DEFERREDSHADING, _RendererLayer)
		, m_Viewport(.0f, .0f, static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<float>(TE_INSTANCE_APPLICATION->GetClientHeight()), .0f, 1.0f)
		, m_ViewRect(0l, 0l, static_cast<long>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientHeight()))
	{}

	void RenderPass_DeferredShading::OnAttach()
	{
		m_RendererCommand.Init(
			TE_IDX_RENDERPASS::DEFERREDSHADING,
			TE_IDX_SHADERCLASS::DEFERREDSHADING,
			TE_INSTANCE_BUFFERMANAGER,
			TE_INSTANCE_SHADERMANAGER);

		InitTexture();
		//InitBuffer(); no buffer is created

		RegisterEvents();

		PreparePipeline();

	}
	void RenderPass_DeferredShading::OnDetach()
	{
		m_RendererCommand.Release();
	}
	void RenderPass_DeferredShading::OnImGuiRender()
	{
	}
	void RenderPass_DeferredShading::OnUpdate(double _DeltaTime)
	{
	}
	void RenderPass_DeferredShading::BeginScene()
	{
		m_RendererCommand.BeginGraphics(&m_Pipeline);

		m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);
		m_RendererCommand.SetRenderTarget(m_RTVSceneBuffer);
		
	}
	void RenderPass_DeferredShading::EndScene()
	{
		m_RendererCommand.End();
	}
	void RenderPass_DeferredShading::Render()
	{
		m_RendererCommand.ExecutePendingCommands();

		m_RendererCommand.Draw(4, 0);
	}
	void RenderPass_DeferredShading::InitTexture()
	{
		m_RendererCommand.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, &m_RTVSceneBuffer);
	}
	void RenderPass_DeferredShading::InitBuffer()
	{
	}
	void RenderPass_DeferredShading::PreparePipeline()
	{
		constexpr RendererStateSet _RendererStates = InitRenderStates(
			TE_RENDERER_STATE_ENABLED_SHADER_HS_FALSE,
			TE_RENDERER_STATE_ENABLED_SHADER_DS_FALSE,
			TE_RENDERER_STATE_ENABLED_SHADER_GS_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_NORMAL_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_SPECULAR_FALSE,
			TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE_FALSE,
			TE_RENDERER_STATE_ENABLED_DEPTH_FALSE,
			TE_RENDERER_STATE_ENABLED_STENCIL_FALSE,
			TE_RENDERER_STATE_FILL_MODE_SOLID,
			TE_RENDERER_STATE_CULL_MODE_BACK,
			TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			TE_RENDERER_STATE_COMPARISSON_FUNC_LESS
		);


		Shader* shader = nullptr;
		auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::DEFERREDSHADING, TE_IDX_MESH_TYPE::MESH_POINT, _RendererStates, "Assets/Shaders/DeferredShading.hlsl", "vs", "ps");

		TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R8G8B8A8_UNORM };

		PipelineGraphics::Factory(&m_Pipeline, _RendererStates, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);
	}
	void RenderPass_DeferredShading::RegisterEvents()
	{
		auto lambda_OnSceneViewportResize = [this](Event& _Event)
		{
			OnSceneViewportResize(static_cast<EventRendererViewportResize&>(_Event));
		};

		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, lambda_OnSceneViewportResize);
	}
	void RenderPass_DeferredShading::OnSceneViewportResize(EventRendererViewportResize& _Event)
	{

		m_Viewport.Resize(static_cast<float>(_Event.GetWidth()), static_cast<float>(_Event.GetHeight()));
		m_ViewRect = ViewRect{ 0l, 0l, static_cast<long>(_Event.GetWidth()), static_cast<long>(_Event.GetHeight()) };

		m_RendererCommand.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, &m_RTVSceneBuffer);
	}
}