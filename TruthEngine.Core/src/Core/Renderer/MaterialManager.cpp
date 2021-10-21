#include "pch.h"
#include "MaterialManager.h"


#include "Core/Application.h"

#include "Core/Event/EventEntity.h"

#include "Core/Renderer/BufferManager.h"


namespace TruthEngine
{

	MaterialManager::MaterialManager()
		: m_BufferManager(BufferManager::GetInstance())
	{}

	Material* MaterialManager::AddMaterial(
		RendererStateSet states
		, const float4& colorDiffuse
		, float roughness
		, float metallic
		, float ambientOcclusion
		, float emission
		, const float2& uvScale
		, const float2& uvTranslate
		, uint32_t diffuseMapIndex
		, uint32_t normalMapIndex
		, uint32_t displacementMapIndex
		, uint32_t specularMapIndex
		, uint32_t roughnessMapIndex
		, uint32_t metallicMapIndex
		, uint32_t ambientOcclusionMapIndex
		, int32_t extraDepthBias
		, float extraSlopeScaledDepthBias
		, float extraDepthBiasClamp
		, TE_IDX_MESH_TYPE meshType
		, bool enabledSSR
		, bool enabledEnvMapReflection
	)
	{
		auto ID = static_cast<uint32_t>(m_Map_Materials.size());

		auto material = std::make_shared<Material>(ID, states, colorDiffuse, roughness, metallic, ambientOcclusion, emission, uvScale, uvTranslate, diffuseMapIndex, normalMapIndex, displacementMapIndex, specularMapIndex, roughnessMapIndex, metallicMapIndex, ambientOcclusionMapIndex, extraDepthBias, extraSlopeScaledDepthBias, extraDepthBiasClamp, meshType, enabledSSR, enabledEnvMapReflection);

		m_Map_Materials[ID] = material;
		m_Materials.push_back(material.get());

		EventEntityAddMaterial event(material.get());

		TE_INSTANCE_APPLICATION->OnEvent(event);

		return material.get();
	}

	TruthEngine::Material* MaterialManager::AddMaterial(Material* material)
	{
		auto ID = static_cast<uint32_t>(m_Map_Materials.size());

		auto _newMaterial = std::make_shared<Material>(ID, material->m_RendererStates, material->m_ColorDiffuse, material->m_Roughness, material->m_Metallic, material->m_AmbientOccclusion, material->m_Emission, material->m_UVScale, material->m_UVTranslate, material->m_MapIndexDiffuse, material->m_MapIndexNormal, material->m_MapIndexDisplacement, material->m_MapIndexSpecular, material->m_MapIndexRoughness, material->m_MapIndexMetallic, material->m_MapIndexAmbientOcclusion, material->m_ExtraDepthBias, material->m_ExtraSlopeScaledDepthBias, material->m_ExtraDepthBiasClamp, material->m_MeshType, material->m_EnabledSSR, material->m_EnabledEnvironmentMapReflection);

		m_Map_Materials[ID] = _newMaterial;
		m_Materials.push_back(_newMaterial.get());

		EventEntityAddMaterial event(_newMaterial.get());

		TE_INSTANCE_APPLICATION->OnEvent(event);

		return _newMaterial.get();
	}

	TruthEngine::Material* MaterialManager::AddDefaultMaterial(TE_IDX_MESH_TYPE meshType)
	{
		auto ID = static_cast<uint32_t>(m_Map_Materials.size());

		auto material = std::make_shared<Material>(
			ID
			, InitRenderStates()
			, float4{ static_cast<float>((float)rand() / (float)RAND_MAX), static_cast<float>((float)rand() / (float)RAND_MAX), static_cast<float>((float)rand() / (float)RAND_MAX), 1.0f }
			, 0.5f
			, 0.0f
			, 1.0f
			, 0.0f
			, float2{ 1.0f, 1.0f }
			, float2{ .0f,.0f }
			, -1
			, -1
			, -1
			, -1
			, -1
			, -1
			, -1
			, 0
			, .0f
			, .0f
			, meshType
			, false
			, false
			);

		m_Map_Materials[ID] = material;
		m_Materials.push_back(material.get());

		EventEntityAddMaterial event(material.get());

		TE_INSTANCE_APPLICATION->OnEvent(event);

		return material.get();
	}

}