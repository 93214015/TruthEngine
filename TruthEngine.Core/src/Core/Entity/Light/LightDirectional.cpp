#include "pch.h"
#include "LightDirectional.h"


namespace TruthEngine::Core
{
	LightDirectional::LightDirectional(
		const std::string_view name
		, const float4 diffusecolor
		, const float4 ambientColor
		, const float4 specularColor
		, const float3 direction
		, const float3 position
		, const float lightSize
		, const int castShadow) 
		: ILight(name, &m_DLightData, TE_LIGHT_TYPE::Directional)
		, m_DLightData(diffusecolor, ambientColor, specularColor, direction, lightSize, position, castShadow)
	{
	}


	LightDirectional::LightDirectional() : ILight("", &m_DLightData, TE_LIGHT_TYPE::Directional)
	{
	}
}