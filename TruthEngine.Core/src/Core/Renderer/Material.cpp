#include "pch.h"
#include "Material.h"

namespace TruthEngine
{
	namespace Core
	{

		Material::Material(
			uint32_t ID, RendererStateSet states
			, uint8_t shaderProperties, float4 colorDiffuse
			, float3 fresnelR0, float shininess
			, uint32_t diffuseMapIndex, uint32_t normalMapIndex
			, uint32_t displacementMapIndex, int32_t extraDepthBias
			, float extraSlopeScaledDepthBias, float extraDepthBiasClamp)
			: m_ID(ID), m_RendererStates(states), m_ShaderProperties(shaderProperties)
			, m_ColorDiffuse(colorDiffuse), m_FresnelR0(fresnelR0), m_Shininess(shininess)
			, m_MapIndexDiffuse(diffuseMapIndex), m_MapIndexNormal(normalMapIndex), m_MapIndexDisplacement(displacementMapIndex)
			, m_ExtraDepthBias(extraDepthBias), m_ExtraSlopeScaledDepthBias(extraSlopeScaledDepthBias), m_ExtraDepthBiasClamp(extraDepthBiasClamp)
		{
		}

	}
}