#include "pch.h"
#include "RenderPass_GenerateGBuffers.h"

#include "Core/Renderer/RendererLayer.h"
#include "Core/Renderer/Material.h"
#include "Core/Renderer/MaterialManager.h"

#include "Core/Event/EventApplication.h"
#include "Core/Event/EventRenderer.h"
#include "Core/Event/EventEntity.h"

#include "Core/Entity/Scene.h"
#include "Core/Entity/Camera/CameraManager.h"
#include "Core/Entity/Components/MeshComponent.h"
#include "Core/Entity/Components/TransformComponent.h"
#include "Core/Entity/Components/BoundingBoxComponent.h"
#include "Core/Entity/Components/MaterialComponent.h"

#include "Core/Profiler/GPUEvents.h"

#include <iostream>

namespace TruthEngine
{

	enum GenerateGBuffers_Shader_SettingMask : uint64_t
	{
		GenerateGBuffers_Shader_SettingMask_ShadingModel_PBR = 1 << 0,
		GenerateGBuffers_Shader_SettingMask_MeshType_Skinned = 1 << 1,
		GenerateGBuffers_Shader_SettingMask_Enabled_NormalMap = 1 << 2,
		GenerateGBuffers_Shader_SettingMask_Enabled_DiffueMap = 1 << 3,
		GenerateGBuffers_Shader_SettingMask_Enabled_RoughnessMap = 1 << 4,
		GenerateGBuffers_Shader_SettingMask_Enabled_MetallicMap = 1 << 5,
		GenerateGBuffers_Shader_SettingMask_Enabled_AmbientOcclusionMap = 1 << 6,
		GenerateGBuffers_Shader_SettingMask_Enabled_SpecularMap = 1 << 7,
	};

	static std::pair<uint64_t, std::vector<const wchar_t*>>  GenerateShaderID(const Material* _Material)
	{
		std::pair<uint64_t, std::vector<const wchar_t*>> _Pair{ 0, {} };

		if (_Material->GetShadingModel() == TE_RENDERER_STATE_SHADING_MODEL_PBR)
		{
			_Pair.first |= GenerateGBuffers_Shader_SettingMask_ShadingModel_PBR;
		}
		if (_Material->GetMeshType() == TE_IDX_MESH_TYPE::MESH_SKINNED)
		{
			_Pair.first |= GenerateGBuffers_Shader_SettingMask_MeshType_Skinned;
			_Pair.second.emplace_back(L"MESH_TYPE_SKINNED");
		}
		if (_Material->GetMapIndexNormal() != -1)
		{
			_Pair.first |= GenerateGBuffers_Shader_SettingMask_Enabled_NormalMap;
			_Pair.second.emplace_back(L"ENABLE_MAP_NORMAL");
		}
		if (_Material->GetMapIndexDiffuse() != -1)
		{
			_Pair.first |= GenerateGBuffers_Shader_SettingMask_Enabled_DiffueMap;
			_Pair.second.emplace_back(L"ENABLE_MAP_DIFFUSE");
		}
		if (_Material->GetMapIndexRoughness() != -1)
		{
			_Pair.first |= GenerateGBuffers_Shader_SettingMask_Enabled_RoughnessMap;
			_Pair.second.emplace_back(L"ENABLE_MAP_ROUGHNESS");
		}
		if (_Material->GetMapIndexMetallic() != -1)
		{
			_Pair.first |= GenerateGBuffers_Shader_SettingMask_Enabled_MetallicMap;
			_Pair.second.emplace_back(L"ENABLE_MAP_METALLIC");
		}
		if (_Material->GetMapIndexAmbientOcclusion() != -1)
		{
			_Pair.first |= GenerateGBuffers_Shader_SettingMask_Enabled_AmbientOcclusionMap;
			_Pair.second.emplace_back(L"ENABLE_MAP_AMBIENTOCCLUSION");
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
			TE_ASSERT_CORE(false, "RenderPass Generate GBuffers: Mesh Type is not identified");
			break;
		}

	}




	RenderPass_GenerateGBuffers::RenderPass_GenerateGBuffers(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::GENERATEGBUFFERS, _RendererLayer)
	{
		//At the moment I've used vector as a container and referece to its element by pointers at other places.
		// so, for prevent from pointers being invalidated in case of vector relocations, I reserve vector space beforehand(the simplest and probably temporary solution).
		m_ContainerPipelines.reserve(1000);
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

		Profiler::GPUEvent::BeginEvent(m_RendererCommand, "GenerateGBuffers");

		m_RendererCommand.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());

		m_RendererCommand.SetRenderTarget(m_RenderTargetViewGBufferColor);
		m_RendererCommand.SetRenderTarget(m_RenderTargetViewGBufferNormal);
		m_RendererCommand.SetRenderTarget(m_RenderTargetViewGBufferSpecular);

		m_RendererCommand.ClearRenderTarget(m_RenderTargetViewGBufferColor);
		m_RendererCommand.ClearRenderTarget(m_RenderTargetViewGBufferNormal);
		m_RendererCommand.ClearRenderTarget(m_RenderTargetViewGBufferSpecular);

		m_RendererCommand.SetDepthStencil(m_RendererLayer->GetDepthStencilViewScene());
	}

	void RenderPass_GenerateGBuffers::EndScene()
	{
		Profiler::GPUEvent::EndEvent(m_RendererCommand);

		m_RendererCommand.End();
	}

	void RenderPass_GenerateGBuffers::Render()
	{
		m_RendererCommand.ExecutePendingCommands();

		auto _CBData_PerMesh = m_ConstantBufferDirect_PerMesh->GetData();

		const Scene* _ActiveScene = GetActiveScene();

		const BoundingFrustum& _CameraBoundingFrustum = TE_INSTANCE_CAMERAMANAGER->GetActiveCamera()->GetBoundingFrustumWorldSpace();

		const auto _EntityMeshView = _ActiveScene->ViewEntities<MeshComponent>();

		for (const auto _Entity : _EntityMeshView)
		{
			const auto& _Transform = _ActiveScene->GetComponent<TransformComponent>(_Entity).GetTransform();
			const auto _XMTransform = Math::ToXM(_Transform);

			const auto& _BoundingBoxLocal = _ActiveScene->GetComponent<BoundingBoxComponent>(_Entity).GetBoundingBox();
			const auto _BoundingBoxWorld = Math::XMTransformBoundingBox(_BoundingBoxLocal, _XMTransform);

			if (_CameraBoundingFrustum.Contains(_BoundingBoxWorld) == ContainmentType::DISJOINT)
			{
				continue;
			}

			auto _MaterialID = _ActiveScene->GetComponent<MaterialComponent>(_Entity).GetMaterial()->GetID();

			*_CBData_PerMesh = ConstantBuffer_Data_Per_Mesh(_Transform, Math::InverseTranspose(_XMTransform), _MaterialID);

			m_RendererCommand.SetPipelineGraphics(m_MapMaterialPipeline[_MaterialID]);
			m_RendererCommand.SetDirectConstantGraphics(m_ConstantBufferDirect_PerMesh);

			const Mesh* _Mesh = &_ActiveScene->GetComponent<MeshComponent>(_Entity).GetMesh();
			m_RendererCommand.DrawIndexed(_Mesh);
		}
	}

	void RenderPass_GenerateGBuffers::InitRendererCommand()
	{
		m_RendererCommand.Init(TE_IDX_RENDERPASS::GENERATEGBUFFERS, TE_IDX_SHADERCLASS::GENERATEGBUFFERS);
	}

	void RenderPass_GenerateGBuffers::InitTextures()
	{
		Application* _Application = TE_INSTANCE_APPLICATION;

		auto _ColorTextureFormat = Settings::Graphics::IsEnabledHDR() ? TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT : TE_RESOURCE_FORMAT::R8G8B8A8_UNORM;

		m_TextureGBufferColor = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Color
			, _Application->GetSceneViewportWidth()
			, _Application->GetSceneViewportHeight()
			, 1
			, _ColorTextureFormat
			, ClearValue_RenderTarget{ .0f, .0f, .0f, 1.0f }
			, true
			, false);

		m_TextureGBufferNormal = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Normal
			, _Application->GetSceneViewportWidth()
			, _Application->GetSceneViewportHeight()
			, 1
			, TE_RESOURCE_FORMAT::R11G11B10_FLOAT
			, ClearValue_RenderTarget{ .0f, .0f, .0f, 1.0f }
			, true
			, false);

		m_TextureGBufferSpecular = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_GBuffer_Specular
			, _Application->GetSceneViewportWidth()
			, _Application->GetSceneViewportHeight()
			, 1
			, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT
			, ClearValue_RenderTarget{ .0f, .0f, .0f, 0.0f }
			, true
			, false);


		m_RendererCommand.CreateRenderTargetView(m_TextureGBufferColor, &m_RenderTargetViewGBufferColor);
		m_RendererCommand.CreateRenderTargetView(m_TextureGBufferNormal, &m_RenderTargetViewGBufferNormal);
		m_RendererCommand.CreateRenderTargetView(m_TextureGBufferSpecular, &m_RenderTargetViewGBufferSpecular);

	}

	void RenderPass_GenerateGBuffers::InitBuffers()
	{
		m_ConstantBufferDirect_PerMesh = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>(TE_IDX_GRESOURCES::Constant_PerMesh_GBuffers);
	}

	void RenderPass_GenerateGBuffers::InitPipelines()
	{
		for (const auto* mat : TE_INSTANCE_MATERIALMANAGER->GetMaterials())
		{
			InitPipelines(mat);
		}
	}

	void RenderPass_GenerateGBuffers::ReleaseRendererCommand()
	{
		m_RendererCommand.Release();
	}

	void RenderPass_GenerateGBuffers::ReleaseTextures()
	{
		m_RendererCommand.ReleaseResource(m_TextureGBufferColor);
		m_RendererCommand.ReleaseResource(m_TextureGBufferNormal);
		m_RendererCommand.ReleaseResource(m_TextureGBufferSpecular);
	}

	void RenderPass_GenerateGBuffers::ReleaseBuffers()
	{
	}

	void RenderPass_GenerateGBuffers::ReleasePipelines()
	{
		m_MapMaterialPipeline.clear();
		m_ContainerPipelines.clear();
	}

	void RenderPass_GenerateGBuffers::RegisterEventListeners()
	{
		auto lambda_OnAddMaterial = [this](Event& event) {
			this->OnAddMaterial(static_cast<EventEntityAddMaterial&>(event));
		};
		m_EventListeners.push_back(TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::EntityAddMaterial, lambda_OnAddMaterial));


		auto lambda_OnUpdateMaterial = [this](Event& event) {
			this->OnUpdateMaterial(static_cast<const EventEntityUpdateMaterial&>(event));
		};
		m_EventListeners.push_back(TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::EntityUpdatedMaterial, lambda_OnUpdateMaterial));

		auto lambda_OnSceneViewportResize = [this](Event& event)
		{
			this->OnRendererViewportResize(static_cast<const EventRendererViewportResize&>(event));
		};
		m_EventListeners.push_back(TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, lambda_OnSceneViewportResize));
	}

	void RenderPass_GenerateGBuffers::UnRegisterEventListeners()
	{
		TE_INSTANCE_APPLICATION->UnRegisterEventListener(m_EventListeners.data(), m_EventListeners.size());
	}

	void RenderPass_GenerateGBuffers::InitPipelines(const Material* _Material)
	{

		const RendererStateSet _RendererState = _Material->GetRendererStates();
		
		auto _ShadingModel = _Material->GetShadingModel();
		auto _LambdaGetShaderFilePath = [_ShadingModel]() 
		{
			switch (_ShadingModel)
			{
			case TE_RENDERER_STATE_SHADING_MODEL_BLINNPHONG:
				return "Assets/Shaders/GenerateGBuffers.hlsl";
			case TE_RENDERER_STATE_SHADING_MODEL_PBR:
				return "Assets/Shaders/GenerateGBuffersPBR.hlsl";
			default:
				TE_LOG_CORE_ERROR("GenerateGBuffer pass: the shading model is invalid!");
			}
		};

		const auto [_ShaderUniqueID, _Macros] = GenerateShaderID(_Material);

		const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::GENERATEGBUFFERS, _ShaderUniqueID, _LambdaGetShaderFilePath(), "vs", "ps", "", "", "", "", _Macros);

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

		const TE_RESOURCE_FORMAT _ColorFormat = Settings::Graphics::IsEnabledHDR() ? m_RendererLayer->GetFormatRenderTargetSceneHDR() : m_RendererLayer->GetFormatRenderTargetSceneSDR();

		const TE_RESOURCE_FORMAT rtvFormats[] = { _ColorFormat , TE_RESOURCE_FORMAT::R11G11B10_FLOAT, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT };

		const auto& _InputElements = GetInputElements(_Material->GetMeshType());

		PipelineGraphics::Factory(_Pipeline, _RendererState, _ShaderHandle, static_cast<uint32_t>(_countof(rtvFormats)), rtvFormats, m_RendererLayer->GetFormatDepthStencilSceneDSV(), _InputElements, false);
	}

	void RenderPass_GenerateGBuffers::OnRendererViewportResize(const EventRendererViewportResize& _Event)
	{
		uint32_t _NewWidth = _Event.GetWidth();
		uint32_t _NewHeight = _Event.GetHeight();


		m_RendererCommand.ResizeRenderTarget(m_TextureGBufferColor, _NewWidth, _NewHeight, &m_RenderTargetViewGBufferColor, nullptr);
		m_RendererCommand.ResizeRenderTarget(m_TextureGBufferNormal, _NewWidth, _NewHeight, &m_RenderTargetViewGBufferNormal, nullptr);
		m_RendererCommand.ResizeRenderTarget(m_TextureGBufferSpecular, _NewWidth, _NewHeight, &m_RenderTargetViewGBufferSpecular, nullptr);

	}

	void RenderPass_GenerateGBuffers::OnAddMaterial(const EventEntityAddMaterial& _Event)
	{
		InitPipelines(_Event.GetMaterial());
	}

	void RenderPass_GenerateGBuffers::OnUpdateMaterial(const EventEntityUpdateMaterial& _Event)
	{
		Material* _Material = _Event.GetMaterial();

		if (auto _ItrPipeline = m_MapMaterialPipeline.find(_Material->GetID()); _ItrPipeline != m_MapMaterialPipeline.end())
		{
			if (_ItrPipeline->second->GetStates() == _Material->GetRendererStates())
				return;
		}

		InitPipelines(_Material);
	}
}