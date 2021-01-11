#include "pch.h"
#include "MaterialManager.h"


#include "Core/Application.h"
#include "Core/Event/EventEntity.h"


namespace TruthEngine::Core
{


	MaterialManager::MaterialManager()
	{
	}

	void MaterialManager::Init(BufferManager* bufferManager)
	{
		m_BufferManager = bufferManager;
	}

	Material* MaterialManager::AddMaterial(
		RendererStateSet states
		, float4 colorDiffuse
		, float3 fresnelR0
		, float shininess
		, uint32_t diffuseMapIndex
		, uint32_t normalMapIndex
		, uint32_t displacementMapIndex
		, int32_t extraDepthBias
		, float extraSlopeScaledDepthBias
		, float extraDepthBiasClamp
		, TE_IDX_MESH_TYPE meshType)
	{
		auto ID = static_cast<uint32_t>(m_Map_Materials.size());

		auto material = std::make_shared<Material>(ID, states, colorDiffuse, fresnelR0, shininess, diffuseMapIndex, normalMapIndex, displacementMapIndex, extraDepthBias, extraSlopeScaledDepthBias, extraDepthBiasClamp, meshType);

		m_Map_Materials[ID] = material;
		m_Materials.push_back(material.get());

		EventEntityAddMaterial event(material.get());

		TE_INSTANCE_APPLICATION->OnEvent(event);

		return material.get();
	}

	void MaterialManager::AddSampleMaterial()
	{
		AddMaterial(InitRenderStates(), float4{ 1.0f, 0.0f, 0.0f, 1.0f }, float3{ 0.3f, 0.3f, 0.3f }, 0.7f, -1, -1, -1, 0, 0.0f, 0.0f, TE_IDX_MESH_TYPE::MESH_NTT);
	}


}