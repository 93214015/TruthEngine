#include "pch.h"
#include "RenderPass_ForwardRendering.h"

#include "Core/Entity/Model/Model3D.h"
#include "Core/Entity/Model/Mesh.h"
#include "Core/Entity/Model/ModelManager.h"
#include "Core/Entity/Components.h"

#include "Core/Renderer/Material.h"
#include "Core/Renderer/Pipeline.h"

#include "Core/Application.h"
#include "Core/Renderer/SwapChain.h"

#include "Core/Event/EventEntity.h"


namespace TruthEngine::Core
{

	RenderPass_ForwardRendering::RenderPass_ForwardRendering()
		: RenderPass(TE_IDX_RENDERPASS::FORWARDRENDERING)
		, m_TextureDepthStencil(nullptr)
		, m_Viewport{ 0.0f, 0.0f, static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<float>(TE_INSTANCE_APPLICATION->GetClientHeight()), 0.0f, 1.0f }
		, m_ViewRect{ static_cast<long>(0.0), static_cast<long>(0.0), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientHeight()) }
		, m_ConstantBufferDirect_PerMesh(nullptr)
	{};

	RenderPass_ForwardRendering::~RenderPass_ForwardRendering() = default;

	RenderPass_ForwardRendering::RenderPass_ForwardRendering(const RenderPass_ForwardRendering& renderer3D) = default;

	RenderPass_ForwardRendering& RenderPass_ForwardRendering::operator=(const RenderPass_ForwardRendering& renderer3D) = default;


	void RenderPass_ForwardRendering::OnAttach()
	{
		m_BufferMgr = TE_INSTANCE_BUFFERMANAGER.get();

		m_ShaderMgr = TE_INSTANCE_SHADERMANAGER.get();

		m_RendererCommand.Init(TE_IDX_RENDERPASS::FORWARDRENDERING, TE_IDX_SHADERCLASS::FORWARDRENDERING, m_BufferMgr, m_ShaderMgr);

		m_TextureDepthStencil = m_RendererCommand.CreateDepthStencil(TE_IDX_TEXTURE::DS_SCENEBUFFER, TE_INSTANCE_APPLICATION->GetClientWidth(), TE_INSTANCE_APPLICATION->GetClientHeight(), TE_RESOURCE_FORMAT::R32_TYPELESS, ClearValue_DepthStencil{ 1.0f, 0 }, false);

		m_RendererCommand.CreateDepthStencilView(m_TextureDepthStencil, &m_DepthStencilView);

		m_RendererCommand.CreateRenderTargetView(TE_IDX_TEXTURE::RT_SCENEBUFFER, &m_RenderTartgetView);
		m_ConstantBufferDirect_PerMesh = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>(TE_IDX_CONSTANTBUFFER::DIRECT_PER_MESH);


		for (const auto* mat : TE_INSTANCE_MODELMANAGER->GetMaterials())
		{
			PreparePiplineMaterial(mat);
		}


		RegisterOnEvents();
	}

	void RenderPass_ForwardRendering::OnDetach()
	{
		m_RendererCommand.Release();
		m_RendererCommand.ReleaseResource(m_TextureDepthStencil);
		m_MaterialPipelines.clear();
	}

	void RenderPass_ForwardRendering::OnImGuiRender()
	{
		ImGui::Begin("RenderPass: Forward Rendering");

		if(ImGui::BeginTable("##forwardrenderingtable", 3, ImGuiTableFlags_ColumnsWidthStretch))
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

		ImGui::End();
	}


	void RenderPass_ForwardRendering::BeginScene()
	{
		//m_TimerBegin.Start();

		m_RendererCommand.Begin();

		m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);
		//m_RendererCommand.SetRenderTarget(TE_INSTANCE_SWAPCHAIN, m_RenderTartgetView);
		m_RendererCommand.SetRenderTarget(m_RenderTartgetView);
		m_RendererCommand.SetDepthStencil(m_DepthStencilView);
		m_RendererCommand.ClearRenderTarget(m_RenderTartgetView);
		m_RendererCommand.ClearDepthStencil(m_DepthStencilView);

		//m_TimerBegin.End();
	}


	void RenderPass_ForwardRendering::EndScene()
	{
	}


	void RenderPass_ForwardRendering::Render()
	{

		m_TotalVertexNum = 0;
		m_TotalMeshNum = 0;

		m_TimerRender.Start();

		auto data = m_ConstantBufferDirect_PerMesh->GetData();

		auto scene = Application::GetApplication()->GetActiveScene();

		auto activeScene = Application::GetApplication()->GetActiveScene();

		auto& reg = activeScene->GetEntityRegistery();


		auto& entityGroup = reg.view<MeshComponent>();


		for (auto entity_mesh : entityGroup)
		{
			float4x4 meshTransform = activeScene->CalcTransformsToRoot(entity_mesh);

			Mesh* mesh = activeScene->GetComponent<MeshComponent>(entity_mesh).GetMesh();
			Material* material = activeScene->GetComponent<MaterialComponent>(entity_mesh).GetMaterial();

			*data = ConstantBuffer_Data_Per_Mesh(meshTransform, Math::InverseTranspose(meshTransform), material->GetID());

			m_RendererCommand.UploadData(m_ConstantBufferDirect_PerMesh);
			m_RendererCommand.SetPipeline(m_MaterialPipelines[material->GetID()].get());
			m_RendererCommand.DrawIndexed(mesh);

			m_TotalVertexNum += mesh->GetVertexNum();
			m_TotalMeshNum++;
		}

		m_RendererCommand.End();

		m_TimerRender.End();
	}


	void RenderPass_ForwardRendering::OnSceneViewportResize(uint32_t width, uint32_t height)
	{

		m_RendererCommand.ResizeDepthStencil(m_TextureDepthStencil, width, height, &m_DepthStencilView, nullptr);
		m_RendererCommand.CreateRenderTargetView(TE_IDX_TEXTURE::RT_SCENEBUFFER, &m_RenderTartgetView);
		m_RendererCommand.CreateDepthStencilView(m_TextureDepthStencil, &m_DepthStencilView);

		m_Viewport.Resize(width, height);

		m_ViewRect = ViewRect{ 0, 0, static_cast<long>(width), static_cast<long>(height) };
	}

	void RenderPass_ForwardRendering::PreparePiplineMaterial(const Material* material)
	{
		std::string shaderName = std::string("renderer3D_material") + std::to_string(material->GetID());

		Shader* shader = nullptr;

		auto result = m_ShaderMgr->AddShader(&shader, TE_IDX_SHADERCLASS::FORWARDRENDERING, material->GetMeshType(), material->GetRendererStates(), "Assets/Shaders/renderer3D.hlsl", "vs", "ps");

		static TE_RESOURCE_FORMAT rtvFormats[1] = { TE_RESOURCE_FORMAT::R8G8B8A8_UNORM };

		auto pipeline = std::make_shared<Pipeline>(material->GetRendererStates(), shader, 1, rtvFormats);

		m_MaterialPipelines[material->GetID()] = pipeline;
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


}