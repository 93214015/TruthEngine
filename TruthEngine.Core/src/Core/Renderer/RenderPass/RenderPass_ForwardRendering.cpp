#include "pch.h"
#include "RenderPass_ForwardRendering.h"

#include "Core/Entity/Model/Model3D.h"
#include "Core/Entity/Model/Mesh.h"
#include "Core/Entity/Model/ModelManager.h"
#include "Core/Entity/Components.h"

#include "Core/Renderer/Material.h"
#include "Core/Renderer/Pipeline.h"

#include "Core/Renderer/RendererLayer.h"

#include "Core/Application.h"
#include "Core/Renderer/SwapChain.h"

#include "Core/Event/EventEntity.h"
#include "Core/Event/EventApplication.h"
#include "Core/Entity/Camera/CameraManager.h"


namespace TruthEngine
{

	RenderPass_ForwardRendering::RenderPass_ForwardRendering(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::FORWARDRENDERING, _RendererLayer)
		, m_TextureDepthStencil(nullptr)
		, m_Viewport{ 0.0f, 0.0f, static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<float>(TE_INSTANCE_APPLICATION->GetClientHeight()), 0.0f, 1.0f }
		, m_ViewRect{ static_cast<long>(0.0), static_cast<long>(0.0), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientHeight()) }
		, m_ConstantBufferDirect_PerMesh(nullptr)
		, m_MaterialManager(MaterialManager::GetInstance())
	{};

	RenderPass_ForwardRendering::~RenderPass_ForwardRendering() = default;

	RenderPass_ForwardRendering::RenderPass_ForwardRendering(const RenderPass_ForwardRendering& renderer3D) = default;

	RenderPass_ForwardRendering& RenderPass_ForwardRendering::operator=(const RenderPass_ForwardRendering& renderer3D) = default;


	void RenderPass_ForwardRendering::OnAttach()
	{
		m_BufferMgr = TE_INSTANCE_BUFFERMANAGER;

		m_ShaderMgr = TE_INSTANCE_SHADERMANAGER;

		m_RendererCommand.Init(TE_IDX_RENDERPASS::FORWARDRENDERING, TE_IDX_SHADERCLASS::FORWARDRENDERING, m_BufferMgr, m_ShaderMgr);
		m_RendererCommand_ResolveTextures.Init(TE_IDX_RENDERPASS::FORWARDRENDERING, TE_IDX_SHADERCLASS::NONE, m_BufferMgr, m_ShaderMgr);

		InitTextures();
		InitBuffers();


		for (const auto* mat : m_MaterialManager->GetMaterials())
		{
			PreparePiplineMaterial(mat);
		}

		PreparePiplineEnvironment();

		RegisterOnEvents();
	}

	void RenderPass_ForwardRendering::OnDetach()
	{
		m_RendererCommand.ReleaseResource(m_TextureDepthStencil);
		m_RendererCommand.Release();
		m_MaterialPipelines.clear();
	}

	void RenderPass_ForwardRendering::OnImGuiRender()
	{
		if (ImGui::Begin("RenderPass: Forward Rendering"))
		{

			if (ImGui::BeginTable("##forwardrenderingtable", 3, ImGuiTableFlags_ColumnsWidthStretch))
			{
				ImGui::TableSetupColumn("Render Time : Draw");
				ImGui::TableSetupColumn("Drawn Mesh Count");
				ImGui::TableSetupColumn("Total Vertex Count");
				ImGui::TableHeadersRow();

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("%.3f ms", m_TimerRender.GetAverageTime());
				//ImGui::Text("FindRootTransform: %.3f ms", m_TimerRender_0.GetAverageTime());
				//ImGui::Text("SetPipelineAndDraw: %.3f ms", m_TimerRender_1.GetAverageTime());


				ImGui::TableNextColumn();
				ImGui::Text("%i", m_TotalMeshNum);

				ImGui::TableNextColumn();
				ImGui::Text("%i", m_TotalVertexNum);

				ImGui::EndTable();
			}

		}
		ImGui::End();
	}


	void RenderPass_ForwardRendering::BeginScene()
	{
		//m_TimerBegin.Start();

		m_RendererCommand.BeginGraphics();
		m_RendererCommand_ResolveTextures.BeginGraphics();

		m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);
		//m_RendererCommand.SetRenderTarget(TE_INSTANCE_SWAPCHAIN, m_RenderTartgetView);
		m_RendererCommand.SetRenderTarget(m_RenderTartgetView);
		m_RendererCommand.SetDepthStencil(m_DepthStencilView);
		m_RendererCommand.ClearRenderTarget(m_RenderTartgetView);
		m_RendererCommand.ClearDepthStencil(m_DepthStencilView);

		if (Settings::IsMSAAEnabled())
		{
			if (m_RendererLayer->IsEnabledHDR())
			{
				m_RendererCommand_ResolveTextures.ResolveMultiSampledTexture(m_TextureRenderTargetHDRMS, TE_IDX_GRESOURCES::Texture_RT_SceneBufferHDR);
			}
			else
			{
				m_RendererCommand_ResolveTextures.ResolveMultiSampledTexture(m_TextureRenderTargetMS, TE_IDX_GRESOURCES::Texture_RT_SceneBuffer);
			}

			m_RendererCommand_ResolveTextures.ResolveMultiSampledTexture(m_TextureDepthStencilMS, m_TextureDepthStencil);
		}


		//m_TimerBegin.End();
	}


	void RenderPass_ForwardRendering::EndScene()
	{
		m_RendererCommand.End();
		m_RendererCommand_ResolveTextures.End();
	}


	void RenderPass_ForwardRendering::Render()
	{

		m_TotalVertexNum = 0;
		m_TotalMeshNum = 0;

		m_TimerRender.Start();

		auto _CBData = m_ConstantBufferDirect_PerMesh->GetData();

		Scene* _ActiveScene = GetActiveScene();

		Camera* _ActiveCamera = CameraManager::GetInstance()->GetActiveCamera();
		const BoundingFrustum& _CameraBoundingFrustum = _ActiveCamera->GetBoundingFrustumWorldSpace();


		auto& EntityMeshView = _ActiveScene->ViewEntities<MeshComponent>();
		for (auto entity_mesh : EntityMeshView)
		{
			const float4x4A& _transform = _ActiveScene->GetComponent<TransformComponent>(entity_mesh).GetTransform();

			const BoundingAABox& _AABB = _ActiveScene->GetComponent<BoundingBoxComponent>(entity_mesh).GetBoundingBox();
			const BoundingAABox _TransformedAABB = Math::TransformBoundingBox(_AABB, _transform);

			if (_CameraBoundingFrustum.Contains(_TransformedAABB) == DirectX::DISJOINT)
				continue;

			const Mesh* mesh = &_ActiveScene->GetComponent<MeshComponent>(entity_mesh).GetMesh();
			const Material* material = _ActiveScene->GetComponent<MaterialComponent>(entity_mesh).GetMaterial();

			*_CBData = ConstantBuffer_Data_Per_Mesh(_transform, Math::InverseTranspose(_transform), material->GetID());

			m_RendererCommand.SetPipelineGraphics(m_MaterialPipelines[material->GetID()]);
			m_RendererCommand.SetDirectConstantGraphics(m_ConstantBufferDirect_PerMesh);
			m_RendererCommand.DrawIndexed(mesh);

			m_TotalVertexNum += mesh->GetVertexNum();
			m_TotalMeshNum++;
		}
		/*}*/

		if (m_RendererLayer->IsEnvironmentMapEnabled())
		{
			auto _EntityViewEnv = _ActiveScene->ViewEntities<EnvironmentComponent>();

			m_RendererCommand.SetPipelineGraphics(m_PipelineEnvironmentCube);
			m_RendererCommand.SetDirectConstantGraphics(m_ConstantBufferDirect_EnvironmentMap);

			for (auto& _EntityEnv : _EntityViewEnv)
			{
				const Mesh* mesh = &_ActiveScene->GetComponent<EnvironmentComponent>(_EntityEnv).GetMesh();
				m_RendererCommand.DrawIndexed(mesh);
			}

		}


		m_TimerRender.End();
	}


	void RenderPass_ForwardRendering::PreparePiplineMaterial(const Material* material)
	{
		std::string shaderName = std::string("renderer3D_material") + std::to_string(material->GetID());

		Shader* shader = nullptr;

		auto result = m_ShaderMgr->AddShader(&shader, TE_IDX_SHADERCLASS::FORWARDRENDERING, material->GetMeshType(), material->GetRendererStates(), "Assets/Shaders/ForwardRendering.hlsl", "vs", "ps");

		TE_RESOURCE_FORMAT rtvFormats[1];

		if (m_RendererLayer->IsEnabledHDR())
		{
			rtvFormats[0] = TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT;
		}
		else
		{
			rtvFormats[0] = TE_RESOURCE_FORMAT::R8G8B8A8_UNORM;
		}

		PipelineGraphics::Factory(&m_MaterialPipelines[material->GetID()], material->GetRendererStates(), shader, 1, rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, true);
	}

	void RenderPass_ForwardRendering::PreparePiplineEnvironment()
	{
		std::string shaderName = std::string("renderer3D_EnvironmentCube");

		Shader* shader = nullptr;

		RendererStateSet states = InitRenderStates();
		SET_RENDERER_STATE(states, TE_RENDERER_STATE_CULL_MODE, TE_RENDERER_STATE_CULL_MODE::TE_RENDERER_STATE_CULL_MODE_NONE);
		SET_RENDERER_STATE(states, TE_RENDERER_STATE_DEPTH_WRITE_MASK, TE_RENDERER_STATE_DEPTH_WRITE_MASK::TE_RENDERER_STATE_DEPTH_WRITE_MASK_ZERO);

		auto result = m_ShaderMgr->AddShader(&shader, TE_IDX_SHADERCLASS::RENDERENVIRONMENTMAP, TE_IDX_MESH_TYPE::MESH_NTT, states, "Assets/Shaders/RenderEnvironmentCube.hlsl", "vs", "ps");

		TE_RESOURCE_FORMAT rtvFormats[1];

		if (m_RendererLayer->IsEnabledHDR())
		{
			rtvFormats[0] = TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT;
		}
		else
		{
			rtvFormats[0] = TE_RESOURCE_FORMAT::R8G8B8A8_UNORM;
		}

		PipelineGraphics::Factory(&m_PipelineEnvironmentCube, states, shader, 1, rtvFormats, TE_RESOURCE_FORMAT::D32_FLOAT, true);
	}

	void RenderPass_ForwardRendering::RegisterOnEvents()
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
			this->OnRenderTargetResize(static_cast<const EventTextureResize&>(event));
		};
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RenderTargetResize, lambda_OnSceneViewportResize);

	}

	void RenderPass_ForwardRendering::OnRenderTargetResize(const EventTextureResize& _Event)
	{
		if (_Event.GetIDX() != TE_IDX_GRESOURCES::Texture_RT_SceneBuffer)
			return;

		uint32_t width = _Event.GetWidth();
		uint32_t height = _Event.GetHeight();

		m_RendererCommand.ResizeDepthStencil(m_TextureDepthStencil, width, height, nullptr, nullptr);

		if (Settings::IsMSAAEnabled())
		{
			m_RendererCommand.ResizeDepthStencil(m_TextureDepthStencilMS, width, height, nullptr, nullptr);
			m_RendererCommand.CreateDepthStencilView(m_TextureDepthStencilMS, &m_DepthStencilView);

			if (m_RendererLayer->IsEnabledHDR())
			{
				m_RendererCommand.ResizeRenderTarget(m_TextureRenderTargetHDRMS, width, height, &m_RenderTartgetView, nullptr);
			}
			else
			{
				m_RendererCommand.ResizeRenderTarget(m_TextureRenderTargetMS, width, height, &m_RenderTartgetView, nullptr);

			}
		}
		else
		{
			if (m_RendererLayer->IsEnabledHDR())
			{
				m_RendererCommand.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBufferHDR, &m_RenderTartgetView);
			}
			else
			{
				m_RendererCommand.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, &m_RenderTartgetView);
			}
			m_RendererCommand.CreateDepthStencilView(m_TextureDepthStencil, &m_DepthStencilView);
		}


		m_Viewport.Resize(width, height);

		m_ViewRect = ViewRect{ 0, 0, static_cast<long>(width), static_cast<long>(height) };
	}

	void RenderPass_ForwardRendering::OnAddMaterial(EventEntityAddMaterial& event)
	{
		PreparePiplineMaterial(event.GetMaterial());
	}

	void RenderPass_ForwardRendering::OnUpdateMaterial(const EventEntityUpdateMaterial& event)
	{

		auto material = event.GetMaterial();

		if (auto itr = m_MaterialPipelines.find(material->GetID()); itr != m_MaterialPipelines.end())
		{
			if (itr->second->GetStates() == material->GetRendererStates())
				return;
		}

		PreparePiplineMaterial(event.GetMaterial());
	}

	void RenderPass_ForwardRendering::InitTextures()
	{
		Application* _App = TE_INSTANCE_APPLICATION;

		ClearValue_RenderTarget _ClearValue{ 0.0f, 0.0f, 0.0f, 1.0f };

		uint32_t _ViewportWidth = TE_INSTANCE_APPLICATION->GetSceneViewportWidth();
		uint32_t _ViewportHeight = TE_INSTANCE_APPLICATION->GetSceneViewportHeight();

		_ViewportWidth = _ViewportWidth != 0 ? _ViewportWidth : 1;
		_ViewportHeight = _ViewportHeight != 0 ? _ViewportHeight : 1;

		m_TextureDepthStencil = m_RendererCommand.CreateDepthStencil(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer, _ViewportWidth, _ViewportHeight, TE_RESOURCE_FORMAT::R32_TYPELESS, ClearValue_DepthStencil{ 1.0f, 0 }, true, false);

		if (Settings::IsMSAAEnabled())
		{
			if (m_RendererLayer->IsEnabledHDR())
			{
				m_TextureRenderTargetHDRMS = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SceneBufferHDRMS, _ViewportWidth, _ViewportHeight, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, _ClearValue, false, true);
				m_RendererCommand.CreateRenderTargetView(m_TextureRenderTargetHDRMS, &m_RenderTartgetView);
			}
			else
			{
				m_TextureRenderTargetMS = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SceneBufferMS, _ViewportWidth, _ViewportHeight, TE_RESOURCE_FORMAT::R8G8B8A8_UNORM, _ClearValue, false, true);
				m_RendererCommand.CreateRenderTargetView(m_TextureRenderTargetMS, &m_RenderTartgetView);
			}

			m_TextureDepthStencilMS = m_RendererCommand.CreateDepthStencil(TE_IDX_GRESOURCES::Texture_DS_SceneBufferMS, _ViewportWidth, _ViewportHeight, TE_RESOURCE_FORMAT::R32_TYPELESS, ClearValue_DepthStencil{ 1.0f, 0 }, false, true);
			m_RendererCommand.CreateDepthStencilView(m_TextureDepthStencilMS, &m_DepthStencilView);

		}
		else
		{
			m_RendererCommand.ReleaseResource(m_TextureRenderTargetMS);
			m_RendererCommand.ReleaseResource(m_TextureRenderTargetHDRMS);
			m_RendererCommand.ReleaseResource(m_TextureDepthStencilMS);

			if (m_RendererLayer->IsEnabledHDR())
			{
				m_RendererCommand.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBufferHDR, &m_RenderTartgetView);
			}
			else
			{
				m_RendererCommand.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, &m_RenderTartgetView);
			}

			m_RendererCommand.CreateDepthStencilView(m_TextureDepthStencil, &m_DepthStencilView);
		}


	}

	void RenderPass_ForwardRendering::InitBuffers()
	{
		m_ConstantBufferDirect_PerMesh = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>(TE_IDX_GRESOURCES::Constant_PerMesh);

		//this constant buffer is created by RendererLayer
		m_ConstantBufferDirect_EnvironmentMap = static_cast<ConstantBufferDirect<ConstantBuffer_Data_EnvironmentMap>*>(m_BufferMgr->GetConstantBufferDirect(TE_IDX_GRESOURCES::Constant_EnvironmentMap));
	}


}
