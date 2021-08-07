#include "pch.h"
#include "RenderPass_GenerateGBuffers.h"

#include "Core/Renderer/RendererLayer.h"
#include "Core/Renderer/Material.h"
#include "Core/Renderer/MaterialManager.h"

#include "Core/Event/EventApplication.h"
#include "Core/Event/EventRenderer.h"
#include "Core/Event/EventEntity.h"

namespace TruthEngine
{
	RenderPass_GenerateGBuffers::RenderPass_GenerateGBuffers(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATEGBUFFERS, _RendererLayer)
		, m_Viewport{ 0.0f, 0.0f, static_cast<float>(TE_INSTANCE_APPLICATION->GetSceneViewportWidth()), static_cast<float>(TE_INSTANCE_APPLICATION->GetSceneViewportHeight()), 0.0f, 1.0f }
		, m_ViewRect{ 0L, 0L, static_cast<long>(TE_INSTANCE_APPLICATION->GetSceneViewportWidth()), static_cast<long>(TE_INSTANCE_APPLICATION->GetSceneViewportHeight()) }
	{
	}

	void RenderPass_GenerateGBuffers::OnAttach()
	{

		m_BufferManager = TE_INSTANCE_BUFFERMANAGER;
		m_ShaderManager = TE_INSTANCE_SHADERMANAGER;

		m_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATEGBUFFERS, TE_IDX_SHADERCLASS::GENERATEGBUFFERS, m_BufferManager, m_ShaderManager);

		InitTextures();
		InitBuffers();

		for (const auto* mat : TE_INSTANCE_MATERIALMANAGER->GetMaterials())
		{
			PreparePipelines(mat);
		}
	}
	void RenderPass_GenerateGBuffers::OnDetach()
	{
		m_RendererCommand.ReleaseResource(m_TextureGBufferColor);
		m_RendererCommand.ReleaseResource(m_TextureGBufferNormal);

		m_MapMaterialPipeline.clear();
		m_ContainerPipelines.clear();
	}
	void RenderPass_GenerateGBuffers::OnImGuiRender()
	{
	}
	void RenderPass_GenerateGBuffers::OnUpdate(double _DeltaTime)
	{
	}
	void RenderPass_GenerateGBuffers::BeginScene()
	{
		m_RendererCommand.BeginGraphics();

		m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);

		m_RendererCommand.SetRenderTarget(m_RenderTargetViewGBufferColor);
		m_RendererCommand.SetRenderTarget(m_RenderTargetViewGBufferNormal);

		m_RendererCommand.ClearRenderTarget(m_RenderTargetViewGBufferColor);
		m_RendererCommand.ClearRenderTarget(m_RenderTargetViewGBufferNormal);

		m_RendererCommand.SetDepthStencil(m_DepthStencilView);
	}
	void RenderPass_GenerateGBuffers::EndScene()
	{
	}
	void RenderPass_GenerateGBuffers::Render()
	{
	}

	void RenderPass_GenerateGBuffers::InitTextures()
	{
		Application* _Application = TE_INSTANCE_APPLICATION;

		m_Viewport.Resize(static_cast<float>(_Application->GetSceneViewportWidth()), static_cast<float>(_Application->GetSceneViewportHeight()));
		m_ViewRect = ViewRect{ 0L, 0L, static_cast<long>(_Application->GetSceneViewportWidth()), static_cast<long>(_Application->GetSceneViewportHeight()) };

		m_TextureGBufferColor = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Color
			, _Application->GetSceneViewportWidth()
			, _Application->GetSceneViewportHeight()
			, TE_RESOURCE_FORMAT::R8G8B8A8_UNORM
			, ClearValue_RenderTarget{ .0f, .0f, .0f, 1.0f }
			, true
			, Settings::IsMSAAEnabled);

		m_TextureGBufferNormal = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Normal
			, _Application->GetSceneViewportWidth()
			, _Application->GetSceneViewportHeight()
			, TE_RESOURCE_FORMAT::R11G11B10_FLOAT
			, ClearValue_RenderTarget{ .0f, .0f, .0f, 1.0f }
			, true
			, Settings::IsMSAAEnabled);

		m_RendererCommand.CreateRenderTargetView(m_TextureGBufferColor, &m_RenderTargetViewGBufferColor);
		m_RendererCommand.CreateRenderTargetView(m_TextureGBufferNormal, &m_RenderTargetViewGBufferNormal);

		m_RendererCommand.CreateDepthStencilView(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer, &m_DepthStencilView);
	}

	void RenderPass_GenerateGBuffers::InitBuffers()
	{
		m_ConstantBufferDirect_PerMesh = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>(TE_IDX_GRESOURCES::CBuffer_PerMesh_GBuffers);
	}

	void RenderPass_GenerateGBuffers::PreparePipelines(const Material* _Material)
	{
		Shader* shader = nullptr;

		auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::GENERATEGBUFFERS, _Material->GetMeshType(), _Material->GetRendererStates(), "Assets/Shaders/GenerateGBuffers.hlsl", "vs", "ps");

		TE_RESOURCE_FORMAT rtvFormats[2] = { TE_RESOURCE_FORMAT::R8G8B8A8_UNORM, TE_RESOURCE_FORMAT::R11G11B10_FLOAT };

		PipelineGraphics* _Pipeline = nullptr;

		auto _MaterialID = _Material->GetID();

		auto _ItrPipeline = m_MapMaterialPipeline.find(_MaterialID);
		if (_ItrPipeline == m_MapMaterialPipeline.end())
		{
			_Pipeline = &m_ContainerPipelines.emplace_back();
			m_MapMaterialPipeline[_MaterialID] = _Pipeline;
		}
		else
		{
			_Pipeline = _ItrPipeline->second;
		}

		PipelineGraphics::Factory(_Pipeline, _Material->GetRendererStates(), shader, static_cast<uint32_t>(_countof(rtvFormats)), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, true);
	}

	void RenderPass_GenerateGBuffers::RegisterEvents()
	{
		auto lambda_OnAddMaterial = [this](Event& event) {
			this->OnAddMaterial(static_cast<EventEntityAddMaterial&>(event));
		};
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::EntityAddMaterial, lambda_OnAddMaterial);


		auto lambda_OnUpdateMaterial = [this](Event& event) {
			this->OnUpdateMaterial(static_cast<const EventEntityUpdateMaterial&>(event));
		};
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::EntityUpdatedMaterial, lambda_OnUpdateMaterial);

		auto lambda_OnSceneViewportResize = [this](Event& event)
		{
			this->OnRendererViewportResize(static_cast<const EventRendererViewportResize&>(event));
		};
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, lambda_OnSceneViewportResize);
	}

	void RenderPass_GenerateGBuffers::OnRendererViewportResize(const EventRendererViewportResize& _Event)
	{
		uint32_t _NewWidth = _Event.GetWidth();
		uint32_t _NewHeight = _Event.GetHeight();

		m_Viewport.Resize(static_cast<float>(_NewWidth), static_cast<float>(_NewHeight));
		m_ViewRect = ViewRect{ 0L, 0L, static_cast<long>(_NewWidth), static_cast<long>(_NewHeight) };

		m_RendererCommand.ResizeRenderTarget(m_TextureGBufferColor, _NewWidth, _NewHeight, &m_RenderTargetViewGBufferColor, nullptr);
		m_RendererCommand.ResizeRenderTarget(m_TextureGBufferNormal, _NewWidth, _NewHeight, &m_RenderTargetViewGBufferNormal, nullptr);

		m_RendererCommand.CreateDepthStencilView(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer, &m_DepthStencilView);

	}
	void RenderPass_GenerateGBuffers::OnAddMaterial(const EventEntityAddMaterial& _Event)
	{
		PreparePipelines(_Event.GetMaterial());
	}
	void RenderPass_GenerateGBuffers::OnUpdateMaterial(const EventEntityUpdateMaterial& _Event)
	{
		Material* _Material = _Event.GetMaterial();

		if (auto _ItrPipeline = m_MapMaterialPipeline.find(_Material->GetID()); _ItrPipeline != m_MapMaterialPipeline.end())
		{
			if (_ItrPipeline->second->GetStates() == _Material->GetRendererStates())
				return;
		}

		PreparePipelines(_Material);
	}
}