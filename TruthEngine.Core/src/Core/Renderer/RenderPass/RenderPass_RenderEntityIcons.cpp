#include "pch.h"
#include "RenderPass_RenderEntityIcons.h"

#include "Core/Renderer/RendererLayer.h"
#include "Core/Renderer/ShaderManager.h"
#include "Core/Renderer/RendererLayer.h"
#include "Core/Entity/Scene.h"
#include "Core/Entity/Components/EditorEntityIconComponent.h"
#include "Core/Entity/Components/TransformComponent.h"

namespace TruthEngine
{



	RenderPass_RenderEntityIcons::RenderPass_RenderEntityIcons(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::RENDERENTITYICON, _RendererLayer)
	{
	}

	void RenderPass_RenderEntityIcons::OnAttach()
	{
		m_RendererCommand.Init(TE_IDX_RENDERPASS::RENDERENTITYICON, TE_IDX_SHADERCLASS::RENDERENTITYICON);

		InitTextures();
		InitBuffers();
		InitPipeline();

		RegisterOnEvents();
	}

	void RenderPass_RenderEntityIcons::OnDetach()
	{
		ReleaseTextures();
		ReleaseBuffers();
		ReleasePipeline();

		m_RendererCommand.Release();
	}

	void RenderPass_RenderEntityIcons::OnImGuiRender()
	{
	}

	void RenderPass_RenderEntityIcons::OnUpdate(double _DeltaTime)
	{
	}

	void RenderPass_RenderEntityIcons::BeginScene()
	{
		m_RendererCommand.BeginGraphics(&m_Pipeline);

		m_RendererCommand.SetRenderTarget(m_RTV);
		m_RendererCommand.SetDepthStencil(m_DSV);
		m_RendererCommand.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
	}

	void RenderPass_RenderEntityIcons::EndScene()
	{
		m_RendererCommand.End();
	}

	void RenderPass_RenderEntityIcons::Render()
	{
		m_RendererCommand.ExecutePendingCommands();

		Scene* _ActiveScene = GetActiveScene();

		auto _ViewEntitiesIcons = _ActiveScene->ViewEntities<EditorEntityIconComponent>();

		auto _CBData = m_ConstantBuffer->GetData();

		for (auto _Entity : _ViewEntitiesIcons)
		{
			auto& _IconComponent = _ActiveScene->GetComponent<EditorEntityIconComponent>(_Entity);
			XMMatrix _XMTransform = Math::ToXM(_ActiveScene->GetComponent<TransformComponent>(_Entity).GetTransform());

			auto _XMCenter = Math::XMTransformPoint(XMVectorOrigin, _XMTransform);

			_CBData->Center = Math::FromXM3(_XMCenter);
			_CBData->TextureIndex = _IconComponent.GetTextureMaterialIndex();
			_CBData->Extents = float3(1.0f, 1.0f, 1.0f);

			m_RendererCommand.SetDirectConstantGraphics(m_ConstantBuffer);

			m_RendererCommand.Draw(1, 0);
		}
	}

	void RenderPass_RenderEntityIcons::InitTextures()
	{
		m_RendererCommand.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, &m_RTV);
		m_RendererCommand.CreateDepthStencilView(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer, &m_DSV);
	}

	void RenderPass_RenderEntityIcons::InitBuffers()
	{
		m_ConstantBuffer = m_RendererCommand.CreateConstantBufferDirect<ConstantBufferData_RenderEntityIcon>(TE_IDX_GRESOURCES::Constant_RenderEntityIcon);
	}

	void RenderPass_RenderEntityIcons::InitPipeline()
	{
		RendererStateSet _RendererStates = InitRenderStates(
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
			TE_RENDERER_STATE_ENABLED_DEPTH_TRUE,
			TE_RENDERER_STATE_ENABLED_STENCIL_FALSE,
			TE_RENDERER_STATE_FILL_MODE_SOLID,
			TE_RENDERER_STATE_CULL_MODE_NONE,
			TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_POINTLIST,
			TE_RENDERER_STATE_COMPARISSON_FUNC_LESS,
			TE_RENDERER_STATE_DEPTH_WRITE_MASK_ALL,
			TE_RENDERER_STATE_ENABLED_HDR_FALSE,
			TE_RENDERER_STATE_SHADING_MODEL_NONE,
			TE_RENDERER_STATE_ENABLED_BLEND_TRUE
		);

		Shader* shader = nullptr;
		auto result = TE_INSTANCE_SHADERMANAGER->AddShader(&shader, TE_IDX_SHADERCLASS::RENDERENTITYICON, TE_IDX_MESH_TYPE::MESH_POINT, _RendererStates, "Assets/Shaders/RenderEntityIcon.hlsl", "vs", "ps", "", "", "", "gs");

		TE_RESOURCE_FORMAT rtvFormats[] = { TE_RESOURCE_FORMAT::R8G8B8A8_UNORM };

		PipelineBlendMode _BlendMode{TE_BLEND::SRC_ALPHA, TE_BLEND::INV_SRC_ALPHA, TE_BLEND_OP::ADD, TE_BLEND::ZERO, TE_BLEND::ONE, TE_BLEND_OP::ADD, TE_COLOR_WRITE_ENABLE_ALL };

		PipelineGraphics::Factory(&m_Pipeline, _RendererStates, shader, _countof(rtvFormats), rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, false, _BlendMode);
	}

	void RenderPass_RenderEntityIcons::ReleaseTextures()
	{
	}

	void RenderPass_RenderEntityIcons::ReleaseBuffers()
	{
	}

	void RenderPass_RenderEntityIcons::ReleasePipeline()
	{
	}

	void RenderPass_RenderEntityIcons::RegisterOnEvents()
	{
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, [this](Event& _Event) 
			{
				OnEventRendererViewportResize(static_cast<EventRendererViewportResize&>(_Event));
			}
		);
	}

	void RenderPass_RenderEntityIcons::OnEventRendererViewportResize(EventRendererViewportResize& _Event)
	{
		InitTextures();
	}

}