#include "pch.h"
#include "LightManager.h"

#include "LightDirectional.h"

namespace TruthEngine::Core
{
	LightDirectional* LightManager::AddLightDirectional(const std::string_view name, const float4 diffusecolor, const float4 ambientColor, const float4 specularColor, const float3 direction, const float3 position, const float lightSize, const int castShadow)
	{
		auto itr = m_Map_DirectionalLights.find(name);
		if (itr != m_Map_DirectionalLights.end())
		{
			return itr->second.get();
		}

		auto dlight = std::make_shared<LightDirectional>(name, diffusecolor, ambientColor, specularColor, direction, position, lightSize, castShadow);
		m_Map_DirectionalLights[name] = dlight;
		
		return dlight.get();
	}

	LightDirectional* LightManager::GetDirectionalLight(const std::string_view name)
	{
		auto itr = m_Map_DirectionalLights.find(name);
		if (itr != m_Map_DirectionalLights.end())
		{
			return itr->second.get();
		}
		else
		{
			return nullptr;
		}
	}
}