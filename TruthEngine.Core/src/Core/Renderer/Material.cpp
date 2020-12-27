#include "pch.h"
#include "Material.h"

namespace TruthEngine
{
	namespace Core
	{

		Material::Material(
			uint32_t ID, RendererStateSet states
			, uint8_t shaderProperties, float4 colorDiffuse
			, float4 colorAmbient, float4 colorSpecular
			, MaterialTexture* diffuseMap, MaterialTexture* normalMap
			, MaterialTexture* displacementMap, int32_t extraDepthBias
			, float extraSlopeScaledDepthBias, float extraDepthBiasClamp)
			: m_ID(ID), m_RendererStates(states), m_ShaderProperties(shaderProperties)
			, m_ColorDiffuse(colorDiffuse), m_ColorAmbient(colorAmbient), m_ColorSpecular(colorSpecular)
			, m_MapDiffuse(diffuseMap), m_MapNormal(normalMap), m_MapDisplacement(displacementMap)
			, m_ExtraDepthBias(extraDepthBias), m_ExtraSlopeScaledDepthBias(extraSlopeScaledDepthBias), m_ExtraDepthBiasClamp(extraDepthBiasClamp)
		{
		}

	}
}