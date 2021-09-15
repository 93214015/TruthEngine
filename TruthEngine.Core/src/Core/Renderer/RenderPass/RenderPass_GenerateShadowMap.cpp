#include "pch.h"
#include "RenderPass_GenerateShadowMap.h"

#include "Core/Renderer/ShaderManager.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Application.h"
#include "Core/Entity/Light/LightManager.h"
#include "Core/Entity/Camera/Camera.h"
#include "Core/Entity/Camera/CameraManager.h"
#include "Core/Entity/Components.h"

namespace TruthEngine
{

	enum GenerateShadowMap_Shader_SettingMask : uint64_t
	{
		GenerateShadowMap_Shader_SettingMask_MeshType_Skinned = 1 << 0,
	};

	static std::pair<uint64_t, std::vector<const wchar_t*>>  GenerateShaderID(TE_IDX_MESH_TYPE _MeshType)
	{
		std::pair<uint64_t, std::vector<const wchar_t*>> _Pair;

		if (_MeshType == TE_IDX_MESH_TYPE::MESH_SKINNED)
		{
			_Pair.first |= GenerateShadowMap_Shader_SettingMask_MeshType_Skinned;
			_Pair.second.emplace_back(L"MESH_TYPE_SKINNED");
		}

		return _Pair;
	}

	static const std::vector<ShaderInputElement>& GetInputElements(TE_IDX_MESH_TYPE _MeshType)
	{
		static const std::vector<ShaderInputElement> _ArrayInputElements[] =
		{
			{
				{"POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0},
			},
			{
				{"POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0},
				{"BONEWEIGHT", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 2, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0},
				{"BONEINDEX", 0, TE_RESOURCE_FORMAT::R8G8B8A8_UINT, 2, 12, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0}
			}
		};

		switch (_MeshType)
		{
		case TE_IDX_MESH_TYPE::MESH_NTT:
			return _ArrayInputElements[0];
			break;
		case TE_IDX_MESH_TYPE::MESH_SKINNED:
			return _ArrayInputElements[1];
			break;
		default:
			TE_ASSERT_CORE(false, "RenderPass ForwardRendering: Mesh Type is not identified");
			break;
		}

	}


	RenderPass_GenerateShadowMap::RenderPass_GenerateShadowMap(RendererLayer* _RendererLayer, uint32_t _ShadowMapSize)
		: RenderPass(TE_IDX_RENDERPASS::GENERATEBASICSHADOWMAP, _RendererLayer)
		, m_ShadoWMapSize(_ShadowMapSize)
		, m_Viewport(Viewport(.0f, .0f, m_ShadoWMapSize, m_ShadoWMapSize, 0.0f, 1.0f))
		, m_ViewRect(ViewRect(0, 0, m_ShadoWMapSize, m_ShadoWMapSize))
	{
		m_ContainerPipelines.reserve(4);
	}

	void RenderPass_GenerateShadowMap::OnImGuiRender()
	{
		if (ImGui::Begin("RenderPass GenerateShadowMap"))
		{

			ImGui::Text("Begin Time : %0.3f ms", m_TimerBegin.GetAverageTime());
			ImGui::Text("Render Time : %0.3f ms", m_TimerRender.GetAverageTime());
			ImGui::Text("Render Time - Prepare Mesh List : %0.3f ms", m_TimerRender_PrepareMeshList.GetAverageTime());
			ImGui::Text("Render Time - Prepare Command List : %0.3f ms", m_TimerRender_PrepareRenderCommand.GetAverageTime());

		}
		ImGui::End();
	}

	void RenderPass_GenerateShadowMap::OnUpdate(double _DeltaTime)
	{
	}

	void RenderPass_GenerateShadowMap::BeginScene()
	{
		m_TimerBegin.Start();

		m_RendererCommand.BeginGraphics();
		m_RendererCommand.SetDepthStencil(m_DepthStencilView_SunLight);
		m_RendererCommand.ClearDepthStencil(m_DepthStencilView_SunLight);

		m_RendererCommand_SpotLights.BeginGraphics();
		m_RendererCommand_SpotLights.SetDepthStencil(m_DepthStencilView_SpotLight);
		m_RendererCommand_SpotLights.ClearDepthStencil(m_DepthStencilView_SpotLight);

		m_TimerBegin.End();
	}

	void RenderPass_GenerateShadowMap::EndScene()
	{
		m_RendererCommand.End();
		m_RendererCommand_SpotLights.End();
	}



	void RenderPass_GenerateShadowMap::Render()
	{
		m_TimerRender.Start();

		RenderSpotLightShadowMap();


		m_RendererCommand.ExecutePendingCommands();


		auto _SunLight = TE_INSTANCE_LIGHTMANAGER->GetDirectionalLight("SunLight");
		if (_SunLight)
		{
			auto cameraCascaded = _SunLight->GetCamera();
			const auto scene = GetActiveScene();
			const auto camera = scene->GetActiveCamera();


			auto data_perMesh = m_ConstantBufferDirect_PerMesh->GetData();
			auto data_perLight = m_ConstantBufferDirect_PerLight->GetData();

			//auto& dynamicEntityGroup = reg.group<MeshComponent, PhysicsDynamicComponent>();
			//auto& staticEntityGroup = reg.view<MeshComponent>(entt::exclude<PhysicsDynamicComponent>);
			//auto& EntityMeshView = scene->ViewEntities<MeshComponent>();
			//auto& EntityModelView = scene->ViewEntities<ModelComponent>();


			m_Viewport.Width = m_ShadoWMapSize * .5f;
			m_Viewport.Height = m_ShadoWMapSize * .5f;

			struct MeshMaterialTransform
			{
				MeshMaterialTransform(const Mesh* _mesh, const float4x4A& _transform, const Material* _material)
					: mMesh(_mesh), mTransform(_transform), mMaterial(_material)
				{}

				const Mesh* mMesh;
				const Material* mMaterial;
				float4x4A mTransform;
			};

			m_TimerRender_PrepareMeshList.Start();

			static std::vector<MeshMaterialTransform> _MeshList;
			_MeshList.clear();
			_MeshList.reserve(10000);

			uint32_t _CascadeNum = cameraCascaded->GetSplitNum();

			static std::vector<std::vector<MeshMaterialTransform*>> _EntityLists(_CascadeNum);
			for (auto& v : _EntityLists)
			{
				v.reserve(1000);
				v.clear();
			}

			const auto EntityMeshView = scene->ViewEntities<MeshComponent>();

			/*for (auto _EntityModel : EntityModelView)
			{

				for (auto& entity_mesh : scene->GetComponent<ModelComponent>(_EntityModel).GetMeshEntities())
				{*/

			for (auto entity_mesh : EntityMeshView)
			{

				/*const float4x4A _transform = scene->GetTransformHierarchy(entity_mesh);*/
				const float4x4A& _transform = scene->GetComponent<TransformComponent>(entity_mesh).GetTransform();


				const Mesh* mesh = &scene->GetComponent<MeshComponent>(entity_mesh).GetMesh();
				const Material* material = scene->GetComponent<MaterialComponent>(entity_mesh).GetMaterial();

				MeshMaterialTransform& _MMT = _MeshList.emplace_back(mesh, _transform, material);

				BoundingAABox _AABB = Math::TransformBoundingBox(scene->GetComponent<BoundingBoxComponent>(entity_mesh).GetBoundingBox(), _transform);
				for (size_t i = 0; i < _CascadeNum; ++i)
				{
					auto _CameraAABB = cameraCascaded->GetBoundingBox(i);

					auto _containment = _CameraAABB.Contains(_AABB);
					if (_containment != DirectX::ContainmentType::DISJOINT)
					{
						_EntityLists[i].emplace_back(&_MMT);
					}
				}
			}

			/*}*/

			m_TimerRender_PrepareMeshList.End();


			m_TimerRender_PrepareRenderCommand.Start();

			uint32_t _CascadeIndex = 0;
			for (auto& _VMeshMaterialTransform : _EntityLists)
			{
				if (_VMeshMaterialTransform.size() == 0)
				{
					_CascadeIndex++;
					continue;
				}

				*data_perLight = ConstantBuffer_Data_Per_Light(cameraCascaded->GetViewProj(_CascadeIndex));
				m_RendererCommand.SetDirectConstantGraphics(m_ConstantBufferDirect_PerLight);

				auto rowIndex = _CascadeIndex % 2;
				auto columnIndex = _CascadeIndex / 2;

				m_Viewport.TopLeftX = static_cast<float>(rowIndex) * (m_ShadoWMapSize * .5f);
				m_Viewport.TopLeftY = static_cast<float>(columnIndex) * (m_ShadoWMapSize * .5f);

				m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);

				for (auto _mmt : _VMeshMaterialTransform)
				{
					*data_perMesh = ConstantBuffer_Data_Per_Mesh(_mmt->mTransform);

					m_RendererCommand.SetDirectConstantGraphics(m_ConstantBufferDirect_PerMesh);
					m_RendererCommand.SetPipelineGraphics(m_PipelinesForwardDepth[_mmt->mMaterial->GetMeshType()]);
					m_RendererCommand.DrawIndexed(_mmt->mMesh);
				}

				_CascadeIndex++;
			}

		}

		m_TimerRender_PrepareRenderCommand.End();



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
				AABoundingBox _transformedAABB = Math::TransformBoundingBox(_aabb, physicsTransform);

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
				AABoundingBox _transformedAABB = Math::TransformBoundingBox(_aabb, meshTransform);

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

	void RenderPass_GenerateShadowMap::InitRendererCommand()
	{
		m_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATEBASICSHADOWMAP, TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP);
		m_RendererCommand_SpotLights.Init(TE_IDX_RENDERPASS::GENERATEBASICSHADOWMAP, TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP);
	}

	void RenderPass_GenerateShadowMap::InitTextures()
	{
		m_TextureDepthStencil_SunLight = m_RendererCommand.CreateDepthStencil(TE_IDX_GRESOURCES::Texture_DS_ShadowMap_SunLight, m_ShadoWMapSize, m_ShadoWMapSize, 1, TE_RESOURCE_FORMAT::R32_TYPELESS, ClearValue_DepthStencil{ 1.0f, 0, TE_CLEAR_DEPTH_STENCIL_FLAGS::CLEAR_DEPTH }, true, false);
		m_TextureDepthStencil_SpotLight = m_RendererCommand_SpotLights.CreateDepthStencil(TE_IDX_GRESOURCES::Texture_DS_ShadowMap_SpotLight, m_ShadoWMapSize, m_ShadoWMapSize, 1, TE_RESOURCE_FORMAT::R32_TYPELESS, ClearValue_DepthStencil{ 1.0f, 0, TE_CLEAR_DEPTH_STENCIL_FLAGS::CLEAR_DEPTH }, true, false);

		m_RendererCommand.CreateDepthStencilView(m_TextureDepthStencil_SunLight, &m_DepthStencilView_SunLight);
		m_RendererCommand_SpotLights.CreateDepthStencilView(m_TextureDepthStencil_SpotLight, &m_DepthStencilView_SpotLight);

	}

	void RenderPass_GenerateShadowMap::InitBuffers()
	{
		m_ConstantBufferDirect_PerLight = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_Per_Light>(TE_IDX_GRESOURCES::Constant_ShadowMapPerLight);
		m_ConstantBufferDirect_PerMesh = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>(TE_IDX_GRESOURCES::Constant_ShadowMapPerMesh);
	}

	void RenderPass_GenerateShadowMap::InitPipelines()
	{
		const RendererStateSet _rendererStateReversedDepth = InitRenderStates(
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
			TE_RENDERER_STATE_FILL_MODE_SOLID,
			TE_RENDERER_STATE_CULL_MODE_BACK,
			TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		);

		const RendererStateSet _rendererStateForwardDepth = InitRenderStates(
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
			TE_RENDERER_STATE_FILL_MODE_SOLID,
			TE_RENDERER_STATE_CULL_MODE_BACK,
			TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		);

		PipelineGraphics* _Pipeline = nullptr;

		/////////////////////////////////////////
		//Reversed Depth Pipeline - Static Mesh
		/////////////////////////////////////////
		{
			const auto [_ShaderUniqueID, _Macros] = GenerateShaderID(TE_IDX_MESH_TYPE::MESH_NTT);

			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP, _ShaderUniqueID, "Assets/Shaders/generateShadowMap.hlsl", "vs", "", "", "", "", "", _Macros);


			auto _ItrPipeline = m_PipelinesReveresedDepth.find(TE_IDX_MESH_TYPE::MESH_NTT);

			if (_ItrPipeline == m_PipelinesReveresedDepth.end())
			{
				_Pipeline = &m_ContainerPipelines.emplace_back();
				m_PipelinesReveresedDepth[TE_IDX_MESH_TYPE::MESH_NTT] = _Pipeline;
			}
			else
			{
				_Pipeline = _ItrPipeline->second;
			}

			const PipelineDepthStencilDesc _PipelineDepthStencilDesc{ TE_DEPTH_WRITE_MASK::ALL, TE_COMPARISON_FUNC::GREATER };

			const auto& _InputElements = GetInputElements(TE_IDX_MESH_TYPE::MESH_NTT);

			PipelineGraphics::Factory(_Pipeline, _rendererStateReversedDepth, _ShaderHandle, 0, nullptr, TE_RESOURCE_FORMAT::D32_FLOAT, _InputElements, false, PipelineBlendDesc{}, _PipelineDepthStencilDesc, -30.0, 0.0f, -4.0f);
		}

		/////////////////////////////////////////
		//Reversed Depth Pipeline - Skinned Mesh
		/////////////////////////////////////////
		{
			const auto [_ShaderUniqueID, _Macros] = GenerateShaderID(TE_IDX_MESH_TYPE::MESH_SKINNED);

			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP, _ShaderUniqueID, "Assets/Shaders/generateShadowMap.hlsl", "vs", "", "", "", "", "", _Macros);

			auto _ItrPipeline = m_PipelinesReveresedDepth.find(TE_IDX_MESH_TYPE::MESH_SKINNED);

			if (_ItrPipeline == m_PipelinesReveresedDepth.end())
			{
				_Pipeline = &m_ContainerPipelines.emplace_back();
				m_PipelinesReveresedDepth[TE_IDX_MESH_TYPE::MESH_SKINNED] = _Pipeline;
			}
			else
			{
				_Pipeline = _ItrPipeline->second;
			}

			const PipelineDepthStencilDesc _PipelineDepthStencilDesc{ TE_DEPTH_WRITE_MASK::ALL, TE_COMPARISON_FUNC::GREATER };

			const auto& _InputElements = GetInputElements(TE_IDX_MESH_TYPE::MESH_SKINNED);

			PipelineGraphics::Factory(_Pipeline, _rendererStateReversedDepth, _ShaderHandle, 0, nullptr, TE_RESOURCE_FORMAT::D32_FLOAT, _InputElements, false, PipelineBlendDesc(), _PipelineDepthStencilDesc, -30.0, 0.0f, -4.0f);

		}

		/////////////////////////////////////////
		//Forward Depth Pipeline - Static Mesh
		/////////////////////////////////////////
		{

			const auto [_ShaderUniqueID, _Macros] = GenerateShaderID(TE_IDX_MESH_TYPE::MESH_NTT);

			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP, _ShaderUniqueID, "Assets/Shaders/generateShadowMap.hlsl", "vs", "", "", "", "", "", _Macros);

			auto _ItrPipeline = m_PipelinesForwardDepth.find(TE_IDX_MESH_TYPE::MESH_NTT);

			if (_ItrPipeline == m_PipelinesForwardDepth.end())
			{
				_Pipeline = &m_ContainerPipelines.emplace_back();
				m_PipelinesForwardDepth[TE_IDX_MESH_TYPE::MESH_NTT] = _Pipeline;
			}
			else
			{
				_Pipeline = _ItrPipeline->second;
			}


			const PipelineDepthStencilDesc _PipelineDepthStencilDesc{ TE_DEPTH_WRITE_MASK::ALL, TE_COMPARISON_FUNC::LESS };

			const auto& _InputElements = GetInputElements(TE_IDX_MESH_TYPE::MESH_NTT);

			PipelineGraphics::Factory(_Pipeline, _rendererStateForwardDepth, _ShaderHandle, 0, nullptr, TE_RESOURCE_FORMAT::D32_FLOAT, _InputElements, false, PipelineBlendDesc(), _PipelineDepthStencilDesc, 30.0, 0.0f, 4.0f);
		}

		/////////////////////////////////////////
		//Forward Depth Pipeline - Skinned Mesh
		/////////////////////////////////////////
		{
			const auto [_ShaderUniqueID, _Macros] = GenerateShaderID(TE_IDX_MESH_TYPE::MESH_SKINNED);

			const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::GENERATEBASICSHADOWMAP, _ShaderUniqueID, "Assets/Shaders/generateShadowMap.hlsl", "vs", "", "", "", "", "", _Macros);

			auto _ItrPipeline = m_PipelinesForwardDepth.find(TE_IDX_MESH_TYPE::MESH_SKINNED);

			if (_ItrPipeline == m_PipelinesForwardDepth.end())
			{
				_Pipeline = &m_ContainerPipelines.emplace_back();
				m_PipelinesForwardDepth[TE_IDX_MESH_TYPE::MESH_SKINNED] = _Pipeline;
			}
			else
			{
				_Pipeline = _ItrPipeline->second;
			}

			const PipelineDepthStencilDesc _PipelineDepthStencilDesc{ TE_DEPTH_WRITE_MASK::ALL, TE_COMPARISON_FUNC::LESS };

			const auto& _InputElements = GetInputElements(TE_IDX_MESH_TYPE::MESH_SKINNED);

			PipelineGraphics::Factory(_Pipeline, _rendererStateForwardDepth, _ShaderHandle, 0, nullptr, TE_RESOURCE_FORMAT::D32_FLOAT, _InputElements, false, PipelineBlendDesc(), _PipelineDepthStencilDesc, 30.0, 0.0f, 4.0f);

		}


	}

	void RenderPass_GenerateShadowMap::ReleaseRendererCommand()
	{
		m_RendererCommand.Release();
		m_RendererCommand_SpotLights.Release();
	}

	void RenderPass_GenerateShadowMap::ReleaseTextures()
	{
		m_RendererCommand.ReleaseResource(m_TextureDepthStencil_SunLight);
		m_RendererCommand_SpotLights.ReleaseResource(m_TextureDepthStencil_SpotLight);
	}

	void RenderPass_GenerateShadowMap::ReleaseBuffers()
	{
	}

	void RenderPass_GenerateShadowMap::ReleasePipelines()
	{
	}

	void RenderPass_GenerateShadowMap::RegisterEventListeners()
	{
	}

	void RenderPass_GenerateShadowMap::UnRegisterEventListeners()
	{
	}

	void RenderPass_GenerateShadowMap::RenderSpotLightShadowMap()
	{

		m_RendererCommand_SpotLights.ExecutePendingCommands();


		const LightSpot* _SpotLight = TE_INSTANCE_LIGHTMANAGER->GetSpotLight("SpotLight0");

		if (_SpotLight)
		{
			const Camera* _Camera = TE_INSTANCE_LIGHTMANAGER->GetLightCamera(_SpotLight);
			Scene* scene = GetActiveScene();

			bool _IsReveresedDepth = _Camera->IsReversedDepth();

			auto data_perMesh = m_ConstantBufferDirect_PerMesh->GetData();
			auto data_perLight = m_ConstantBufferDirect_PerLight->GetData();


			Viewport _Viewport(.0f, .0f, m_ShadoWMapSize, m_ShadoWMapSize, 0.0f, 1.0f);
			ViewRect _ViewRect(0L, 0L, (long)m_ShadoWMapSize, (long)m_ShadoWMapSize);
			m_RendererCommand_SpotLights.SetViewPort(&_Viewport, &_ViewRect);


			*data_perLight = ConstantBuffer_Data_Per_Light(_Camera->GetViewProj());
			m_RendererCommand_SpotLights.SetDirectConstantGraphics(m_ConstantBufferDirect_PerLight);

			auto& EntityModelView = scene->ViewEntities<ModelComponent>();
			for (auto _EntityModel : EntityModelView)
			{

				for (auto& entity_mesh : scene->GetComponent<ModelComponent>(_EntityModel).GetMeshEntities())
				{

					/*const float4x4A _transform = scene->GetTransformHierarchy(entity_mesh);*/
					const float4x4A& _transform = scene->GetComponent<TransformComponent>(entity_mesh);

					const Mesh* mesh = &scene->GetComponent<MeshComponent>(entity_mesh).GetMesh();
					const Material* material = scene->GetComponent<MaterialComponent>(entity_mesh).GetMaterial();

					BoundingAABox _AABB = Math::TransformBoundingBox(scene->GetComponent<BoundingBoxComponent>(entity_mesh).GetBoundingBox(), _transform);

					auto _containment = _Camera->GetBoundingFrustumWorldSpace().Contains(_AABB);
					if (_containment != DirectX::ContainmentType::DISJOINT)
					{
						*data_perMesh = ConstantBuffer_Data_Per_Mesh(_transform);

						PipelineGraphics* _Pipeline = nullptr;
						if (_IsReveresedDepth)
							_Pipeline = m_PipelinesReveresedDepth[material->GetMeshType()];
						else
							_Pipeline = m_PipelinesForwardDepth[material->GetMeshType()];

						m_RendererCommand_SpotLights.SetDirectConstantGraphics(m_ConstantBufferDirect_PerMesh);
						m_RendererCommand_SpotLights.SetPipelineGraphics(_Pipeline);
						m_RendererCommand_SpotLights.DrawIndexed(mesh);
					}
				}
			}
		}
	}


}