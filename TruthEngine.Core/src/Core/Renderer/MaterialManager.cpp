#include "pch.h"
#include "MaterialManager.h"


#include "Core/Application.h"
#include "Core/Event/EventEntity.h"


namespace TruthEngine
{


	MaterialManager::MaterialManager()
	{
	}

	void MaterialManager::Init(BufferManager* bufferManager)
	{
		m_BufferManager = bufferManager;

		/*AddMaterial(InitRenderStates()
			, float4{ 0.5f, 0.4f, 0.7f, 1.0f }
			, float3{ 0.3f, .3f, .3f }
			, 1.0f
			, -1
			, -1
			, -1
			, 0
			, .0f
			, .0f
			, TE_IDX_MESH_TYPE::MESH_NTT);

		AddMaterial(InitRenderStates_CCW()
			, float4{ 0.7f, 0.7f, 0.7f, 1.0f }
			, float3{ 0.0f, .0f, .0f }
			, 0.0f
			, -1
			, -1
			, -1
			, 0
			, .0f
			, .0f
			, TE_IDX_MESH_TYPE::MESH_NTT);*/
	}

	Material* MaterialManager::AddMaterial(
		RendererStateSet states
		, const float4& colorDiffuse
		, const float3& fresnelR0
		, float shininess
		, const float2& uvScale
		, const float2& uvTranslate
		, uint32_t diffuseMapIndex
		, uint32_t normalMapIndex
		, uint32_t displacementMapIndex
		, int32_t extraDepthBias
		, float extraSlopeScaledDepthBias
		, float extraDepthBiasClamp
		, TE_IDX_MESH_TYPE meshType)
	{
		auto ID = static_cast<uint32_t>(m_Map_Materials.size());

		auto material = std::make_shared<Material>(ID, states, colorDiffuse, fresnelR0, shininess, uvScale, uvTranslate, diffuseMapIndex, normalMapIndex, displacementMapIndex, extraDepthBias, extraSlopeScaledDepthBias, extraDepthBiasClamp, meshType);

		m_Map_Materials[ID] = material;
		m_Materials.push_back(material.get());

		EventEntityAddMaterial event(material.get());

		TE_INSTANCE_APPLICATION->OnEvent(event);

		return material.get();
	}

	TruthEngine::Material* MaterialManager::AddMaterial(Material* material)
	{
		auto ID = static_cast<uint32_t>(m_Map_Materials.size());

		auto _newMaterial = std::make_shared<Material>(ID, material->m_RendererStates, material->m_ColorDiffuse, material->m_FresnelR0, material->m_Shininess, material->m_UVScale, material->m_UVTranslate, material->m_MapIndexDiffuse, material->m_MapIndexNormal, material->m_MapIndexDisplacement, material->m_ExtraDepthBias, material->m_ExtraSlopeScaledDepthBias, material->m_ExtraDepthBiasClamp, material->m_MeshType);

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
			, float3{ 0.3f, .3f, .3f }
			, 0.0f
			, float2{1.0f, 1.0f}
			, float2{.0f,.0f}
			, -1
			, -1
			, -1
			, 0
			, .0f
			, .0f
			, meshType);

		m_Map_Materials[ID] = material;
		m_Materials.push_back(material.get());

		EventEntityAddMaterial event(material.get());

		TE_INSTANCE_APPLICATION->OnEvent(event);

		return material.get();
	}

}