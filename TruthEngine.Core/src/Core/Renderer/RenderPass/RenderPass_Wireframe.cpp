#include "pch.h"
#include "RenderPass_Wireframe.h"

#include "Core/Renderer/ShaderManager.h"
#include "Core/Renderer/RendererLayer.h"
#include "Core/Entity/Scene.h"
#include "Core/Entity/Camera/Camera.h"
#include "Core/Event/EventRenderer.h"

namespace TruthEngine
{



	RenderPass_Wireframe::RenderPass_Wireframe(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::WIREFRAME, _RendererLayer)
	{}

	void RenderPass_Wireframe::OnAttach()
	{
		InitRendererCommand();

		InitTextures();
		InitBuffers();
		InitPipelines();

		RegiterOnEvents();
	}

	void RenderPass_Wireframe::OnDetach()
	{
		ReleaseTextures();
		ReleaseBuffers();
		ReleasePipelines();

		ReleaseRendererCommand();
	}

	void RenderPass_Wireframe::OnImGuiRender()
	{
	}

	void RenderPass_Wireframe::OnUpdate(double _DeltaTime)
	{
	}

	void RenderPass_Wireframe::BeginScene()
	{
		m_RendererCommand.BeginGraphics(&m_Pipeline);

		m_RendererCommand.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
		m_RendererCommand.SetRenderTarget(m_RTV);
		m_RendererCommand.SetDepthStencil(m_DSV);
	}

	void RenderPass_Wireframe::EndScene()
	{
		m_RendererCommand.End();
	}

	void RenderPass_Wireframe::Render(const Mesh* _Mesh, const float4x4A& _Transform)
	{
		m_RendererCommand.ExecutePendingCommands();

		auto _Camera = GetActiveScene()->GetActiveCamera();

		auto _CBData = m_ConstantBuffer->GetData();

		_CBData->WVP = Math::Multiply(_Transform, _Camera->GetViewProj());

		m_RendererCommand.SetDirectConstantGraphics(m_ConstantBuffer);

		m_RendererCommand.DrawIndexed(_Mesh);
	}

	void RenderPass_Wireframe::InitRendererCommand()
	{
		m_RendererCommand.Init(TE_IDX_RENDERPASS::WIREFRAME, TE_IDX_SHADERCLASS::WIREFRAME);
	}

	void RenderPass_Wireframe::InitTextures()
	{
		m_RendererCommand.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, &m_RTV);
		m_RendererCommand.CreateDepthStencilView(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer, &m_DSV);
	}

	void RenderPass_Wireframe::InitBuffers()
	{
		m_ConstantBuffer = m_RendererCommand.CreateConstantBufferDirect<ConstantBufferData_Wireframe>(TE_IDX_GRESOURCES::Constant_Wireframe);
	}

	void RenderPass_Wireframe::InitPipelines()
	{
		const RendererStateSet _States = InitRenderStates
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
			TE_RENDERER_STATE_ENABLED_DEPTH_TRUE,
			TE_RENDERER_STATE_ENABLED_STENCIL_FALSE,
			TE_RENDERER_STATE_FILL_MODE_WIREFRAME,
			TE_RENDERER_STATE_CULL_MODE_NONE,
			TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			TE_RENDERER_STATE_COMPARISSON_FUNC_LESS,
			TE_RENDERER_STATE_DEPTH_WRITE_MASK_ALL,
			TE_RENDERER_STATE_ENABLED_HDR_FALSE,
			TE_RENDERER_STATE_SHADING_MODEL_NONE,
			TE_RENDERER_STATE_ENABLED_BLEND_FALSE
		);

		Shader* shader = nullptr;
		auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::WIREFRAME, TE_IDX_MESH_TYPE::MESH_SIMPLE, _States, "Assets/Shaders/RenderWireframe.hlsl", "vs", "ps");

		TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R8G8B8A8_UNORM };

		PipelineGraphics::Factory(&m_Pipeline, _States, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false);
	}

	void RenderPass_Wireframe::ReleaseTextures()
	{
	}

	void RenderPass_Wireframe::ReleaseBuffers()
	{
	}

	void RenderPass_Wireframe::ReleasePipelines()
	{
	}

	void RenderPass_Wireframe::ReleaseRendererCommand()
	{
		m_RendererCommand.Release();
	}

	void RenderPass_Wireframe::RegiterOnEvents()
	{
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, [this](Event& _Event) 
			{
				OnEventRendererViewportResize(static_cast<EventRendererViewportResize&>(_Event));
			}
		);
	}

	void RenderPass_Wireframe::OnEventRendererViewportResize(EventRendererViewportResize& _Event)
	{
		InitTextures();
	}

}