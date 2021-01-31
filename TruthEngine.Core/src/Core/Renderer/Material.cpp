#include "pch.h"
#include "Material.h"

#include "Core/Application.h"
#include "Core/Event/EventEntity.h"
#include "Core/Entity/Model/ModelManager.h"

namespace TruthEngine
{
	namespace Core
	{

		Material::Material(
			uint32_t ID, RendererStateSet states
			, const float4& colorDiffuse
			, const float3& fresnelR0, float shininess
			, const float2& uvScale, const float2& uvTranslate
			, uint32_t diffuseMapIndex, uint32_t normalMapIndex
			, uint32_t displacementMapIndex, int32_t extraDepthBias
			, float extraSlopeScaledDepthBias, float extraDepthBiasClamp, TE_IDX_MESH_TYPE meshType)
			: m_ID(ID), m_RendererStates(states)
			, m_ColorDiffuse(colorDiffuse), m_FresnelR0(fresnelR0), m_Shininess(shininess)
			, m_UVScale(uvScale), m_UVTranslate(uvTranslate)
			, m_MapIndexDiffuse(diffuseMapIndex), m_MapIndexNormal(normalMapIndex), m_MapIndexDisplacement(displacementMapIndex)
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

			if (m_MapIndexDiffuse == -1 && index != -1)
			{
				SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_TRUE);
			}
			else if (m_MapIndexDiffuse != -1 && index == -1)
			{
				SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_FALSE);
			}

			m_MapIndexDiffuse = index;

			InvokeEventChangeMaterial();
		}

		void Material::SetMapIndexNormal(uint32_t index)
		{
			if (index == m_MapIndexNormal)
			{
				return;
			}


			if (m_MapIndexNormal == -1 && index != -1)
			{
				SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_NORMAL, TE_RENDERER_STATE_ENABLED_MAP_NORMAL_TRUE);
			}
			else if (m_MapIndexNormal != -1 && index == -1)
			{
				SET_RENDERER_STATE(m_RendererStates, TE_RENDERER_STATE_ENABLED_MAP_NORMAL, TE_RENDERER_STATE_ENABLED_MAP_NORMAL_FALSE);
			}

			m_MapIndexNormal = index;

			InvokeEventChangeMaterial();
		}

		void Material::SetMapIndexDisplacement(uint32_t index)
		{
		}

	}
}