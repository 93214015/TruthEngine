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

namespace TruthEngine
{


	RenderPass_GenerateShadowMap::RenderPass_GenerateShadowMap()
		: RenderPass(TE_IDX_RENDERPASS::GENERATEBASICSHADOWMAP)
		, m_Viewport(Viewport(.0f, .0f, m_ShadoWMapSize, m_ShadoWMapSize, 0.0f, 1.0f))
		, m_ViewRect(ViewRect(0, 0, 2048, 2048))
	{}


	void RenderPass_GenerateShadowMap::OnAttach()
	{
		m_ShaderManager = TE_INSTANCE_SHADERMANAGER;
		m_BufferManager = TE_INSTANCE_BUFFERMANAGER;
		m_LightManager = LightManager::GetInstace();

		m_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATEBASICSHADOWMAP, TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP, m_BufferManager, m_ShaderManager);

		m_TextureDepthStencil = m_RendererCommand.CreateDepthStencil(TE_IDX_TEXTURE::DS_SHADOWMAP, m_ShadoWMapSize, m_ShadoWMapSize, TE_RESOURCE_FORMAT::R32_TYPELESS, ClearValue_DepthStencil{ 0.0f, 0 }, true, false);
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
		if (ImGui::Begin("RenderPass GenerateShadowMap"))
		{

			ImGui::Text("Begin Time : %0.3f", m_TimerBegin.GetAverageTime());
			ImGui::Text("Render Time : %0.3f", m_TimerRender.GetAverageTime());

		}
		ImGui::End();
	}

	void RenderPass_GenerateShadowMap::BeginScene()
	{
		m_TimerBegin.Start();

		m_RendererCommand.Begin();

		//m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);
		//m_RendererCommand.SetRenderTarget(TE_INSTANCE_SWAPCHAIN, m_RenderTartgetView);
		m_RendererCommand.SetDepthStencil(m_DepthStencilView);
		m_RendererCommand.ClearDepthStencil(m_DepthStencilView);

		m_TimerBegin.End();
	}

	void RenderPass_GenerateShadowMap::EndScene()
	{
		m_RendererCommand.End();
	}

	void RenderPass_GenerateShadowMap::Render()
	{
		m_TimerRender.Start();

		auto light = m_LightManager->GetDirectionalLight("dlight_0");
		auto cameraCascaded = m_LightManager->GetLightCameraCascaded(light);
		auto scene = TE_INSTANCE_APPLICATION->GetActiveScene();
		auto camera = CameraManager::GetInstance()->GetActiveCamera();


		auto data_perMesh = m_ConstantBufferDirect_PerMesh->GetData();
		auto data_perLight = m_ConstantBufferDirect_PerLight->GetData();



		//auto& dynamicEntityGroup = reg.group<MeshComponent, PhysicsDynamicComponent>();
		//auto& staticEntityGroup = reg.view<MeshComponent>(entt::exclude<PhysicsDynamicComponent>);
		auto& EntityMeshView = scene->ViewEntities<MeshComponent>();


		m_Viewport.Width = 1024.0f;
		m_Viewport.Height = 1024.0f;

		struct MeshMaterialTransform
		{
			MeshMaterialTransform(const Mesh* _mesh, const float4x4* _transform, const Material* _material)
				: mMesh(_mesh), mTransform(_transform), mMaterial(_material)
			{}

			const Mesh* mMesh;
			const float4x4* mTransform;
			const Material* mMaterial;
		};

		std::vector<std::vector<MeshMaterialTransform>> _EntityLists;
		_EntityLists.resize(cameraCascaded->GetSplitNum());
		for (auto& v : _EntityLists)
		{
			v.reserve(1000);
		}

		std::vector<BoundingFrustum> _CascadedBoundingFrustums;
		_CascadedBoundingFrustums.reserve(cameraCascaded->GetSplitNum());

		for (uint32_t cascadeIndex = 0; cascadeIndex < cameraCascaded->GetSplitNum(); ++cascadeIndex)
		{
			BoundingFrustum& _bf = _CascadedBoundingFrustums.emplace_back(cameraCascaded->GetBoundingFrustum(cascadeIndex));
			auto temp = _bf.Near;
			_bf.Near = _bf.Far;
			_bf.Far = temp;
		}


		for (auto entity_mesh : EntityMeshView)
		{
			const float4x4* transform = nullptr;
			if (scene->HasComponent<PhysicsDynamicComponent>(entity_mesh))
			{
				transform = &scene->GetComponent<PhysicsDynamicComponent>(entity_mesh).GetTranform();
			}
			else
			{
				transform = &scene->GetComponent<TransformComponent>(entity_mesh).GetTransform();
			}

			const auto& _aabb = scene->GetComponent<BoundingBoxComponent>(entity_mesh).GetBoundingBox();
			BoundingBox _transformedAABB = Math::TransformBoundingBox(_aabb, *transform);


			uint32_t i = 0;
			for (auto& _BoundingFrustum : _CascadedBoundingFrustums)
			{
				auto _ContainmentResult = _BoundingFrustum.Contains(_transformedAABB);
				if (_ContainmentResult != DirectX::DISJOINT)
				{
					const Mesh* mesh = scene->GetComponent<MeshComponent>(entity_mesh).GetMesh();
					const Material* material = scene->GetComponent<MaterialComponent>(entity_mesh).GetMaterial();
					_EntityLists[i].emplace_back(mesh, transform, material);
					//If this frustum contain the mesh fully then don't need to go further and check on next frustums
					/*if (_ContainmentResult == DirectX::CONTAINS)
					{
						break;
					}*/
				}
				//Check the next frustum of cascaded camera
				++i;
			}

		}

		uint32_t _CascadeIndex = 0;
		for (auto& _VMeshMaterialTransform : _EntityLists)
		{
			if (_VMeshMaterialTransform.size() == 0)
			{
				_CascadeIndex++;
				continue;
			}

			*data_perLight = ConstantBuffer_Data_Per_Light(cameraCascaded->GetViewProj(_CascadeIndex));
			m_RendererCommand.UploadData(m_ConstantBufferDirect_PerLight);

			auto rowIndex = _CascadeIndex % 2;
			auto columnIndex = _CascadeIndex / 2;

			m_Viewport.TopLeftX = static_cast<float>(rowIndex) * 1024.0f;
			m_Viewport.TopLeftY = static_cast<float>(columnIndex) * 1024.0f;

			m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);

			for (auto& _mmt : _VMeshMaterialTransform)
			{
				*data_perMesh = ConstantBuffer_Data_Per_Mesh(*_mmt.mTransform);

				m_RendererCommand.UploadData(m_ConstantBufferDirect_PerMesh);
				m_RendererCommand.SetPipeline(m_Pipelines[_mmt.mMaterial->GetMeshType()].get());
				m_RendererCommand.DrawIndexed(_mmt.mMesh);
			}

			_CascadeIndex++;
		}

		/*for (uint32_t cascadeIndex = 0; cascadeIndex < cameraCascaded->GetSplitNum(); ++cascadeIndex)
		{
			//auto camera = CameraManager::GetInstance()->GetActiveCamera();
			// *data_perLight = ConstantBuffer_Data_Per_Light(camera->GetViewProj());
			*data_perLight = ConstantBuffer_Data_Per_Light(cameraCascaded->GetViewProj(cascadeIndex));
			m_RendererCommand.UploadData(m_ConstantBufferDirect_PerLight);

			auto rowIndex = cascadeIndex % 2;
			auto columnIndex = cascadeIndex / 2;

			m_Viewport.TopLeftX = static_cast<float>(rowIndex) * 1024.0f;
			m_Viewport.TopLeftY = static_cast<float>(columnIndex) * 1024.0f;

			m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);

			auto _boundingFrustum = cameraCascaded->GetBoundingFrustum(cascadeIndex);
			auto temp = _boundingFrustum.Near;
			_boundingFrustum.Near = _boundingFrustum.Far;
			_boundingFrustum.Far = temp;

			for (auto entity_mesh : dynamicEntityGroup)
			{
				const float4x4& physicsTransform = scene->GetComponent<PhysicsDynamicComponent>(entity_mesh).GetTranform();
				const auto& _aabb = scene->GetComponent<BoundingBoxComponent>(entity_mesh).GetBoundingBox();
				BoundingBox _transformedAABB = Math::TransformBoundingBox(_aabb, physicsTransform);

				if (_boundingFrustum.Contains(_transformedAABB) == DirectX::DISJOINT)
					continue;

				Mesh* mesh = scene->GetComponent<MeshComponent>(entity_mesh).GetMesh();
				Material* material = scene->GetComponent<MaterialComponent>(entity_mesh).GetMaterial();

				*data_perMesh = ConstantBuffer_Data_Per_Mesh(physicsTransform);

				m_RendererCommand.UploadData(m_ConstantBufferDirect_PerMesh);
				m_RendererCommand.SetPipeline(m_Pipelines[material->GetMeshType()].get());
				m_RendererCommand.DrawIndexed(mesh);
			}


			for (auto entity_mesh : staticEntityGroup)
			{
				const float4x4& meshTransform = scene->GetComponent<TransformComponent>(entity_mesh).GetTransform();
				const auto& _aabb = scene->GetComponent<BoundingBoxComponent>(entity_mesh).GetBoundingBox();
				BoundingBox _transformedAABB = Math::TransformBoundingBox(_aabb, meshTransform);

				const auto c = _boundingFrustum.Contains(_transformedAABB);

				if ( c == DirectX::DISJOINT)
					continue;

				Mesh* mesh = scene->GetComponent<MeshComponent>(entity_mesh).GetMesh();
				Material* material = scene->GetComponent<MaterialComponent>(entity_mesh).GetMaterial();

				*data_perMesh = ConstantBuffer_Data_Per_Mesh(meshTransform);

				m_RendererCommand.UploadData(m_ConstantBufferDirect_PerMesh);
				m_RendererCommand.SetPipeline(m_Pipelines[material->GetMeshType()].get());
				m_RendererCommand.DrawIndexed(mesh);

			}

		}*/

		m_TimerRender.End();

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

		auto pipeline = std::make_shared<Pipeline>(_rendererState, shader, 0, nullptr, TE_RESOURCE_FORMAT::D32_FLOAT, false, -30.0, 0.0f, -4.0f);

		m_Pipelines[TE_IDX_MESH_TYPE::MESH_NTT] = pipeline;

	}


}