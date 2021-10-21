#include "pch.h"
#include "RenderPass_ForwardRendering.h"

#include "Core/Entity/Model/Model3D.h"
#include "Core/Entity/Model/Mesh.h"
#include "Core/Entity/Model/ModelManager.h"
#include "Core/Entity/Components.h"

#include "Core/Renderer/Material.h"

#include "Core/Renderer/RendererLayer.h"

#include "Core/Application.h"
#include "Core/Renderer/SwapChain.h"

#include "Core/Event/EventEntity.h"
#include "Core/Event/EventRenderer.h"
#include "Core/Entity/Camera/CameraManager.h"

#include "Core/Profiler/GPUEvents.h"


namespace TruthEngine
{


	enum ForwardRendering_Shader_SettingMask : uint64_t
	{
		ForwardRendering_Shader_SettingMask_ShadingModel_PBR = 1 << 0,
		ForwardRendering_Shader_SettingMask_MeshType_Skinned = 1 << 1,
		ForwardRendering_Shader_SettingMask_Enabled_NormalMap = 1 << 2,
		ForwardRendering_Shader_SettingMask_Enabled_DiffueMap = 1 << 3,
		ForwardRendering_Shader_SettingMask_Enabled_RoughnessMap = 1 << 4,
		ForwardRendering_Shader_SettingMask_Enabled_MetallicMap = 1 << 5,
		ForwardRendering_Shader_SettingMask_Enabled_AmbientOcclusionMap = 1 << 6,
		ForwardRendering_Shader_SettingMask_Enabled_SpecularMap = 1 << 7,
		ForwardRendering_Shader_SettingMask_HDR = 1 << 8,
	};

	static std::pair<uint64_t, std::vector<const wchar_t*>>  GenerateShaderID(const Material* _Material)
	{
		std::pair<uint64_t, std::vector<const wchar_t*>> _Pair;

		if (_Material->GetShadingModel() == TE_RENDERER_STATE_SHADING_MODEL::TE_RENDERER_STATE_SHADING_MODEL_PBR)
		{
			_Pair.first |= ForwardRendering_Shader_SettingMask_ShadingModel_PBR;
		}
		if (_Material->GetMeshType() == TE_IDX_MESH_TYPE::MESH_SKINNED)
		{
			_Pair.first |= ForwardRendering_Shader_SettingMask_MeshType_Skinned;
			_Pair.second.emplace_back(L"MESH_TYPE_SKINNED");
		}
		if (_Material->GetMapIndexNormal() != -1)
		{
			_Pair.first |= ForwardRendering_Shader_SettingMask_Enabled_NormalMap;
			_Pair.second.emplace_back(L"ENABLE_MAP_NORMAL");
		}
		if (_Material->GetMapIndexDiffuse() != -1)
		{
			_Pair.first |= ForwardRendering_Shader_SettingMask_Enabled_DiffueMap;
			_Pair.second.emplace_back(L"ENABLE_MAP_DIFFUSE");
		}
		if (_Material->GetMapIndexRoughness() != -1)
		{
			_Pair.first |= ForwardRendering_Shader_SettingMask_Enabled_RoughnessMap;
			_Pair.second.emplace_back(L"ENABLE_MAP_ROUGHNESS");
		}
		if (_Material->GetMapIndexMetallic() != -1)
		{
			_Pair.first |= ForwardRendering_Shader_SettingMask_Enabled_MetallicMap;
			_Pair.second.emplace_back(L"ENABLE_MAP_METALLIC");
		}
		if (_Material->GetMapIndexAmbientOcclusion() != -1)
		{
			_Pair.first |= ForwardRendering_Shader_SettingMask_Enabled_AmbientOcclusionMap;
			_Pair.second.emplace_back(L"ENABLE_MAP_AMBIENTOCCLUSION");
		}
		if (Settings::Graphics::IsEnabledHDR())
		{
			_Pair.first |= ForwardRendering_Shader_SettingMask_HDR;
			_Pair.second.emplace_back(L"ENABLE_HDR");
		}

		return _Pair;
	}


	static const std::vector<ShaderInputElement>& GetInputElements(TE_IDX_MESH_TYPE _MeshType)
	{
		static const std::vector<ShaderInputElement> _ArrayInputElements[] =
		{
			{
				{"POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0},
				{"NORMAL", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 1, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0},
				{"TANGENT", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 1, 12, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0},
				{"TEXCOORD", 0, TE_RESOURCE_FORMAT::R32G32_FLOAT, 1, 24, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0},
			},
			{
				{"POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0},
				{"NORMAL", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 1, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0},
				{"TANGENT", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 1, 12, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0},
				{"TEXCOORD", 0, TE_RESOURCE_FORMAT::R32G32_FLOAT, 1, 24, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0},
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


	RenderPass_ForwardRendering::RenderPass_ForwardRendering(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::FORWARDRENDERING, _RendererLayer)
		, m_ConstantBufferDirect_PerMesh(nullptr)
	{
		//At the moment I've used vector as a container and referece to its element by pointers other places.
		// so, for prevent from pointers being invalidated in case of vector relocations, I reserve vector space beforehand(the simplest and probably temporary solution).
		m_ContainerPipelines.reserve(1000);
	};

	void RenderPass_ForwardRendering::OnImGuiRender()
	{
		if (ImGui::Begin("RenderPass: Forward Rendering"))
		{

			if (ImGui::BeginTable("##forwardrenderingtable", 3, ImGuiTableFlags_SizingStretchSame))
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

	void RenderPass_ForwardRendering::OnUpdate(double _DeltaTime)
	{

	}

	void RenderPass_ForwardRendering::BeginScene()
	{
		//m_TimerBegin.Start();

		m_RendererCommand.BeginGraphics();
		TE_GPUBEGINEVENT(m_RendererCommand, "ForwardRendering");

		m_RendererCommand.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
		
		m_RendererCommand.SetRenderTarget(*m_RenderTartgetView);
		m_RendererCommand.SetDepthStencil(*m_DepthStencilView);


		//m_TimerBegin.End();
	}

	void RenderPass_ForwardRendering::EndScene()
	{
		TE_GPUENDEVENT(m_RendererCommand);
		m_RendererCommand.End();
	}

	void RenderPass_ForwardRendering::Render()
	{

		//Execute and clear pending commands
		m_RendererCommand.ExecutePendingCommands();

		m_TotalVertexNum = 0;
		m_TotalMeshNum = 0;

		m_TimerRender.Start();

		auto _CBData = m_ConstantBufferDirect_PerMesh->GetData();

		const Scene* _ActiveScene = GetActiveScene();

		Camera* _ActiveCamera = CameraManager::GetInstance()->GetActiveCamera();
		const BoundingFrustum& _CameraBoundingFrustum = _ActiveCamera->GetBoundingFrustumWorldSpace();


		const auto EntityMeshView = _ActiveScene->ViewEntities<MeshComponent>();

		size_t s = sizeof(EntityMeshView);

		for (auto entity_mesh : EntityMeshView)
		{
			const float4x4A& _transform = _ActiveScene->GetComponent<TransformComponent>(entity_mesh).GetTransform();

			const BoundingAABox& _AABB = _ActiveScene->GetComponent<BoundingBoxComponent>(entity_mesh).GetBoundingBox();
			const BoundingAABox _TransformedAABB = Math::TransformBoundingBox(_AABB, _transform);

			if (_CameraBoundingFrustum.Contains(_TransformedAABB) == ContainmentType::DISJOINT)
				continue;

			const Mesh* mesh = &_ActiveScene->GetComponent<MeshComponent>(entity_mesh).GetMesh();
			auto _MaterialID = _ActiveScene->GetComponent<MaterialComponent>(entity_mesh).GetMaterial()->GetID();

			*_CBData = ConstantBuffer_Data_Per_Mesh(_transform, Math::InverseTranspose(_transform), _MaterialID);

			m_RendererCommand.SetPipelineGraphics(m_MaterialPipelines[_MaterialID]);
			m_RendererCommand.SetDirectConstantGraphics(m_ConstantBufferDirect_PerMesh);
			m_RendererCommand.DrawIndexed(mesh);

			m_TotalVertexNum += mesh->GetVertexNum();
			m_TotalMeshNum++;
		}
		/*}*/

		/*if (m_RendererLayer->IsEnvironmentMapEnabled())
		{
			const auto _EntityViewEnv = _ActiveScene->ViewEntities<EnvironmentComponent>();

			m_RendererCommand.SetPipelineGraphics(&m_PipelineEnvironmentCube);
			m_RendererCommand.SetDirectConstantGraphics(m_ConstantBufferDirect_EnvironmentMap);

			for (auto& _EntityEnv : _EntityViewEnv)
			{
				const Mesh* mesh = &_ActiveScene->GetComponent<EnvironmentComponent>(_EntityEnv).GetMesh();
				m_RendererCommand.DrawIndexed(mesh);
			}

		}*/


		m_TimerRender.End();
	}

	void RenderPass_ForwardRendering::InitPipelines(const Material* material)
	{
		RendererStateSet _RendererState = material->GetRendererStates();

		RendererStateSet _ShadingModel = GET_RENDERER_STATE(_RendererState, TE_RENDERER_STATE_SHADING_MODEL);

		auto _LambdaGetShaderAddress = [_ShadingModel]()
		{
			switch (_ShadingModel)
			{
			case TE_RENDERER_STATE_SHADING_MODEL_NONE:
				TE_LOG_CORE_WARN("RenderPass_ForwardRendering: the shading model of material was 'None'. the blinn-phong is used as default.");
				return "Assets/Shaders/ForwardRendering.hlsl";
			case TE_RENDERER_STATE_SHADING_MODEL_BLINNPHONG:
				return "Assets/Shaders/ForwardRendering.hlsl";
			case TE_RENDERER_STATE_SHADING_MODEL_PBR:
				return "Assets/Shaders/ForwardRenderingPBR.hlsl";
			}
		};

		const char* _ShaderFilePath = _LambdaGetShaderAddress();

		auto [_UniqueShaderID, _Macros] = GenerateShaderID(material);

		auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::FORWARDRENDERING, _UniqueShaderID, _ShaderFilePath, "vs", "ps", "", "", "", "", _Macros);

		PipelineGraphics* _Pipeline = nullptr;

		auto _MaterialID = material->GetID();

		auto _ItrPipeline = m_MaterialPipelines.find(_MaterialID);
		if (_ItrPipeline == m_MaterialPipelines.end())
		{
			_Pipeline = &m_ContainerPipelines.emplace_back();
			m_MaterialPipelines[_MaterialID] = _Pipeline;
		}
		else
		{
			_Pipeline = _ItrPipeline->second;
		}

		const auto& _InputElements = GetInputElements(material->GetMeshType());

		TE_RESOURCE_FORMAT rtvFormats[] = { Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetFormatRenderTargetSceneHDR() : m_RendererLayer->GetFormatRenderTargetSceneSDR() };

		PipelineGraphics::Factory(_Pipeline, _RendererState, _ShaderHandle, _countof(rtvFormats), rtvFormats, m_RendererLayer->GetFormatDepthStencilSceneDSV(), _InputElements, true);

	}

	/*void RenderPass_ForwardRendering::InitPipelines_Environment()
	{
		RendererStateSet states = InitRenderStates();
		SET_RENDERER_STATE(states, TE_RENDERER_STATE_CULL_MODE, TE_RENDERER_STATE_CULL_MODE::TE_RENDERER_STATE_CULL_MODE_NONE);

		const TE_RESOURCE_FORMAT rtvFormats[] = { Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetFormatRenderTargetSceneHDR() : m_RendererLayer->GetFormatRenderTargetSceneSDR() };

		const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::RENDERENVIRONMENTMAP, 0, "Assets/Shaders/RenderEnvironmentCube.hlsl", "vs", "ps");

		const PipelineDepthStencilDesc _PipelineDSDesc{ TE_DEPTH_WRITE_MASK::ZERO, TE_COMPARISON_FUNC::LESS };

		const std::vector<ShaderInputElement> _InputElements{ {"POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0 } };

		PipelineGraphics::Factory(&m_PipelineEnvironmentCube, states, _ShaderHandle, 1, rtvFormats, m_RendererLayer->GetFormatDepthStencilSceneDSV(), _InputElements, true, PipelineBlendDesc{}, _PipelineDSDesc);
	}*/

	void RenderPass_ForwardRendering::RegisterEventListeners()
	{
		auto lambda_OnAddMaterial = [this](Event& event) {
			this->OnAddMaterial(static_cast<EventEntityAddMaterial&>(event));
		};

		m_EventListenerHandles.push_back(TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::EntityAddMaterial, lambda_OnAddMaterial));


		auto lambda_OnUpdateMaterial = [this](Event& event) {
			this->OnUpdateMaterial(static_cast<const EventEntityUpdateMaterial&>(event));
		};

		m_EventListenerHandles.push_back(TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::EntityUpdatedMaterial, lambda_OnUpdateMaterial));
	}

	void RenderPass_ForwardRendering::UnRegisterEventListeners()
	{
		TE_INSTANCE_APPLICATION->UnRegisterEventListener(m_EventListenerHandles.data(), m_EventListenerHandles.size());
	}

	void RenderPass_ForwardRendering::OnAddMaterial(EventEntityAddMaterial& event)
	{
		InitPipelines(event.GetMaterial());
	}

	void RenderPass_ForwardRendering::OnUpdateMaterial(const EventEntityUpdateMaterial& event)
	{

		auto material = event.GetMaterial();

		if (auto itr = m_MaterialPipelines.find(material->GetID()); itr != m_MaterialPipelines.end())
		{
			if (itr->second->GetStates() == material->GetRendererStates())
				return;
		}

		InitPipelines(event.GetMaterial());
	}

	void RenderPass_ForwardRendering::InitRendererCommand()
	{
		m_RendererCommand.Init(TE_IDX_RENDERPASS::FORWARDRENDERING, TE_IDX_SHADERCLASS::FORWARDRENDERING);
	}

	void RenderPass_ForwardRendering::InitTextures()
	{
		m_RenderTartgetView = &(Settings::Graphics::IsEnabledMSAA() ? (Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetRenderTargetViewSceneHDRMS() : m_RendererLayer->GetRenderTargetViewSceneSDRMS()) : (Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetRenderTargetViewSceneHDR() : m_RendererLayer->GetRenderTargetViewSceneSDR()));
		m_DepthStencilView = &(Settings::Graphics::IsEnabledMSAA() ? m_RendererLayer->GetDepthStencilViewSceneMS() : m_RendererLayer->GetDepthStencilViewScene());
	}

	void RenderPass_ForwardRendering::InitBuffers()
	{
		m_ConstantBufferDirect_PerMesh = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>(TE_IDX_GRESOURCES::Constant_PerMesh);

		//this constant buffer is created by RendererLayer
		//m_ConstantBufferDirect_EnvironmentMap = static_cast<ConstantBufferDirect<ConstantBuffer_Data_EnvironmentMap>*>(TE_INSTANCE_BUFFERMANAGER->GetConstantBufferDirect(TE_IDX_GRESOURCES::Constant_EnvironmentMap));
	}

	void RenderPass_ForwardRendering::InitPipelines()
	{
		for (const auto* mat : TE_INSTANCE_MATERIALMANAGER->GetMaterials())
		{
			InitPipelines(mat);
		}

		//InitPipelines_Environment();

	}

	void RenderPass_ForwardRendering::ReleaseRendererCommand()
	{
		m_RendererCommand.Release();
	}

	void RenderPass_ForwardRendering::ReleaseTextures()
	{
	}

	void RenderPass_ForwardRendering::ReleaseBuffers()
	{
	}

	void RenderPass_ForwardRendering::ReleasePipelines()
	{
		m_MaterialPipelines.clear();
		m_ContainerPipelines.clear();
	}


}
