#include "pch.h"
#include "Material.h"

#include "Core/Application.h"
#include "Core/Event/EventEntity.h"
#include "Core/Entity/Model/ModelManager.h"

namespace TruthEngine
{

	Material::Material(
		uint32_t ID, RendererStateSet states
		, const float4& colorDiffuse
		//, const float3& fresnelR0, float shininess
		, float roughness
		, float metallic
		, float ambientOcclusion
		, const float2& uvScale, const float2& uvTranslate
		, uint32_t diffuseMapIndex, uint32_t normalMapIndex
		, uint32_t displacementMapIndex
		, uint32_t specularMapIndex
		, uint32_t roughnessMapIndex
		, uint32_t metallicMapIndex
		, uint32_t ambientOcclusionMapIndex
		, int32_t extraDepthBias
		, float extraSlopeScaledDepthBias, float extraDepthBiasClamp, TE_IDX_MESH_TYPE meshType)
		: m_ID(ID), m_RendererStates(states)
		, m_ColorDiffuse(colorDiffuse)
		//, m_FresnelR0(fresnelR0), m_Shininess(shininess)
		, m_Roughness(roughness)
		, m_Metallic(metallic)
		, m_AmbientOccclusion(ambientOcclusion)
		, m_UVScale(uvScale), m_UVTranslate(uvTranslate)
		, m_MapIndexDiffuse(diffuseMapIndex), m_MapIndexNormal(normalMapIndex), m_MapIndexDisplacement(displacementMapIndex), m_MapIndexSpecular(specularMapIndex)
		, m_MapIndexRoughness(roughnessMapIndex), m_MapIndexMetallic(metallicMapIndex), m_MapIndexAmbientOcclusion(ambientOcclusionMapIndex)
		, m_ExtraDepthBias(extraDepthBias), m_ExtraSlopeScaledDepthBias(extraSlopeScaledDepthBias), m_ExtraDepthBiasClamp(extraDepthBiasClamp)
		, m_MeshType(meshType)
	{}


	void Material::InvokeEventChangeMaterial()
	{
		EventEntityUpdateMaterial event(this);
		Application::GetApplication()->OnEvent(event);
	}

	void Material::SetMapIndexDiffuse(uint32_t index)
	{
		if (index == m_MapIndexNormal)
		{
			return;
		}

		m_MapIndexDiffuse = index;

		if (index != -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_TRUE);
		}
		else if (index == -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_FALSE);
		}


		InvokeEventChangeMaterial();
	}

	void Material::SetMapIndexNormal(uint32_t index)
	{
		if (index == m_MapIndexNormal)
		{
			return;
		}

		m_MapIndexNormal = index;

		if (index != -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_NORMAL, TE_RENDERER_STATE_ENABLED_MAP_NORMAL_TRUE);
		}
		else if (index == -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_NORMAL, TE_RENDERER_STATE_ENABLED_MAP_NORMAL_FALSE);
		}


		InvokeEventChangeMaterial();
	}

	void Material::SetMapIndexDisplacement(uint32_t index)
	{
		if (index == m_MapIndexDisplacement)
		{
			return;
		}

		m_MapIndexDisplacement = index;

		if (index != -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_TRUE);
		}
		else if (index == -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_FALSE);
		}


		InvokeEventChangeMaterial();
	}
	void Material::SetMapIndexSpecular(uint32_t index)
	{
		if (index == m_MapIndexSpecular)
		{
			return;
		}

		m_MapIndexSpecular = index;

		if (index != -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_SPECULAR, TE_RENDERER_STATE_ENABLED_MAP_SPECULAR_TRUE);
		}
		else if (index == -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_SPECULAR, TE_RENDERER_STATE_ENABLED_MAP_SPECULAR_FALSE);
		}


		InvokeEventChangeMaterial();
	}
	void Material::SetMapIndexRoughness(uint32_t index)
	{
		if (index == m_MapIndexRoughness)
		{
			return;
		}

		m_MapIndexRoughness = index;

		if (index != -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_ROUGHNESS, TE_RENDERER_STATE_ENABLED_MAP_ROUGHNESS_TRUE);
		}
		else if (index == -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_ROUGHNESS, TE_RENDERER_STATE_ENABLED_MAP_ROUGHNESS_FALSE);
		}


		InvokeEventChangeMaterial();
	}
	void Material::SetMapIndexMetallic(uint32_t index)
	{
		if (index == m_MapIndexMetallic)
		{
			return;
		}

		m_MapIndexMetallic = index;

		if (index != -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_METALLIC, TE_RENDERER_STATE_ENABLED_MAP_METALLIC_TRUE);
		}
		else if (index == -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_METALLIC, TE_RENDERER_STATE_ENABLED_MAP_METALLIC_FALSE);
		}

		InvokeEventChangeMaterial();
	}
	void Material::SetMapIndexAmbientOcclusion(uint32_t index)
	{
		if (index == m_MapIndexAmbientOcclusion)
		{
			return;
		}

		m_MapIndexAmbientOcclusion = index;

		if (index != -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_AMBIENTOCCLUSION, TE_RENDERER_STATE_ENABLED_MAP_AMBIENTOCCLUSION_TRUE);
		}
		else if (index == -1)
		{
			SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_AMBIENTOCCLUSION, TE_RENDERER_STATE_ENABLED_MAP_AMBIENTOCCLUSION_FALSE);
		}

		InvokeEventChangeMaterial();
	}
}