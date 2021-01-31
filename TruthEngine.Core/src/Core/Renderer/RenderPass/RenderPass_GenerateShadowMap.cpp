#include "pch.h"
#include "RenderPass_GenerateShadowMap.h"

#include "Core/Renderer/ShaderManager.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Application.h"
#include "Core/Entity/Light/LightManager.h"
#include "Core/Entity/Camera/Camera.h"
#include "Core/Renderer/Pipeline.h"
#include "Core/Entity/Camera/CameraManager.h"
#include "Core/Entity/Components.h"

namespace TruthEngine::Core
{


	RenderPass_GenerateShadowMap::RenderPass_GenerateShadowMap()
		: RenderPass(TE_IDX_RENDERPASS::GENERATEBASICSHADOWMAP)
		, m_Viewport(Viewport(.0f, .0f, m_ShadoWMapSize, m_ShadoWMapSize, 0.0f, 1.0f))
		, m_ViewRect(ViewRect(0, 0, m_ShadoWMapSize, m_ShadoWMapSize))
	{}


	void RenderPass_GenerateShadowMap::OnAttach()
	{
		m_ShaderManager = TE_INSTANCE_SHADERMANAGER.get();
		m_BufferManager = TE_INSTANCE_BUFFERMANAGER.get();
		m_LightManager = LightManager::GetInstace();

		m_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATEBASICSHADOWMAP, TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP, m_BufferManager, m_ShaderManager);

		m_TextureDepthStencil = m_RendererCommand.CreateDepthStencil(TE_IDX_TEXTURE::DS_SHADOWMAP, m_ShadoWMapSize, m_ShadoWMapSize, TE_RESOURCE_FORMAT::R32_TYPELESS, ClearValue_DepthStencil{ 0.0f, 0 }, true);
		m_RendererCommand.CreateDepthStencilView(m_TextureDepthStencil, &m_DepthStencilView);

		m_ConstantBufferDirect_PerLight = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_Per_Light>(TE_IDX_CONSTANTBUFFER::DIRECT_SHADOWMAP_PER_LIGHT);
		m_ConstantBufferDirect_PerMesh = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>(TE_IDX_CONSTANTBUFFER::DIRECT_SHADOWMAP_PER_MESH);


		InitPipeline();
	}

	void RenderPass_GenerateShadowMap::OnDetach()
	{
		
	}

	void RenderPass_GenerateShadowMap::OnImGuiRender()
	{
		
	}

	void RenderPass_GenerateShadowMap::BeginScene()
	{
		m_RendererCommand.Begin();

		m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);
		//m_RendererCommand.SetRenderTarget(TE_INSTANCE_SWAPCHAIN, m_RenderTartgetView);
		m_RendererCommand.SetDepthStencil(m_DepthStencilView);
		m_RendererCommand.ClearDepthStencil(m_DepthStencilView);
	}

	void RenderPass_GenerateShadowMap::EndScene()
	{
		
	}

	void RenderPass_GenerateShadowMap::Render()
	{
		auto data_perMesh = m_ConstantBufferDirect_PerMesh->GetData();
		auto data_perLight = m_ConstantBufferDirect_PerLight->GetData();


		auto light = m_LightManager->GetDirectionalLight("dlight_0");


		//auto camera = CameraManager::GetInstance()->GetActiveCamera();
		//*data_perLight = ConstantBuffer_Data_Per_Light(camera->GetViewProj());
		*data_perLight = ConstantBuffer_Data_Per_Light(m_LightManager->GetLightCamera(light)->GetViewProj());
		m_RendererCommand.UploadData(m_ConstantBufferDirect_PerLight);

		auto scene = Application::GetApplication()->GetActiveScene();

		auto activeScene = Application::GetApplication()->GetActiveScene();

		auto& reg = activeScene->GetEntityRegistery();

		auto& dynamicEntityGroup = reg.group<MeshComponent, PhysicsDynamicComponent>();


		for (auto entity_mesh : dynamicEntityGroup)
		{
			const float4x4& physicsTransform = activeScene->GetComponent<PhysicsDynamicComponent>(entity_mesh).GetTranform();

			Mesh* mesh = activeScene->GetComponent<MeshComponent>(entity_mesh).GetMesh();
			Material* material = activeScene->GetComponent<MaterialComponent>(entity_mesh).GetMaterial();

			*data_perMesh = ConstantBuffer_Data_Per_Mesh(physicsTransform);

			m_RendererCommand.UploadData(m_ConstantBufferDirect_PerMesh);
			m_RendererCommand.SetPipeline(m_Pipelines[material->GetMeshType()].get());
			m_RendererCommand.DrawIndexed(mesh);

		}


		auto& staticEntityGroup = reg.view<MeshComponent>(entt::exclude<PhysicsDynamicComponent>);
		

		for (auto entity_mesh : staticEntityGroup)
		{
			const float4x4& meshTransform = activeScene->GetComponent<TransformComponent>(entity_mesh).GetTransform();

			Mesh* mesh = activeScene->GetComponent<MeshComponent>(entity_mesh).GetMesh();
			Material* material = activeScene->GetComponent<MaterialComponent>(entity_mesh).GetMaterial();

			*data_perMesh = ConstantBuffer_Data_Per_Mesh(meshTransform);

			m_RendererCommand.UploadData(m_ConstantBufferDirect_PerMesh);
			m_RendererCommand.SetPipeline(m_Pipelines[material->GetMeshType()].get());
			m_RendererCommand.DrawIndexed(mesh);

		}

		m_RendererCommand.End();
	}

	void RenderPass_GenerateShadowMap::OnSceneViewportResize(uint32_t width, uint32_t height)
	{
	}

	void RenderPass_GenerateShadowMap::InitPipeline()
	{
		static RendererStateSet _rendererState = InitRenderStates(
			TE_RENDERER_STATE_ENABLED_SHADER_HS_FALSE,
			TE_RENDERER_STATE_ENABLED_SHADER_DS_FALSE,
			TE_RENDERER_STATE_ENABLED_SHADER_GS_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_NORMAL_FALSE,
			TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_FALSE,
			TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE_FALSE,
			TE_RENDERER_STATE_ENABLED_DEPTH_TRUE,
			TE_RENDERER_STATE_ENABLED_STENCIL_FALSE,
			TE_RENDERER_STATE_FILL_MODE_SOLID,
			TE_RENDERER_STATE_CULL_MODE_BACK,
			TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			TE_RENDERER_STATE_COMPARISSON_FUNC_GREATER
		);

		Shader* shader = nullptr;
		auto result = m_ShaderManager->AddShader(&shader, TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP, TE_IDX_MESH_TYPE::MESH_NTT, _rendererState, "Assets/Shaders/generateShadowMap.hlsl", "vs", "");

		auto pipeline = std::make_shared<Pipeline>(_rendererState, shader, 0, nullptr, -30.0, 0.0f, -4.0f);

		m_Pipelines[TE_IDX_MESH_TYPE::MESH_NTT] = pipeline;

	}


}