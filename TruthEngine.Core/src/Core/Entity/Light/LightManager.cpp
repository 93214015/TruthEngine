#include "pch.h"
#include "LightManager.h"

#include "LightDirectional.h"

#include "Core/Entity/Camera/CameraManager.h"

constexpr float4x4 m_ProjToUV = float4x4(0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.5, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 1.0f);

namespace TruthEngine::Core
{
	LightDirectional* LightManager::AddLightDirectional(const std::string_view name, const float4 diffusecolor, const float4 ambientColor, const float4 specularColor, const float3 direction, const float3 position, const float lightSize, const int castShadow, const float range)
	{
		auto itr = m_Map_LightsName.find(name);
		if (itr != m_Map_LightsName.end())
		{
			if (itr->second->GetLightType() == TE_LIGHT_TYPE::Directional)
			{
				return static_cast<LightDirectional*>(itr->second);
			}
			else
			{
				TE_LOG_CORE_ERROR("The querried light was exist but the type was wrong!");
				return nullptr;
			}
		}

		uint32_t _id = m_Map_Lights.size();

		auto dlight = &m_LightsDirectional.emplace_back(_id, name, diffusecolor, ambientColor, specularColor, direction, position, lightSize, castShadow, range);
		m_Map_Lights[_id] = dlight;
		m_Map_LightsName[name] = dlight;
		
		return dlight;
	}

	LightDirectional* LightManager::GetDirectionalLight(const std::string_view name)
	{
		auto itr = m_Map_LightsName.find(name);
		if (itr != m_Map_LightsName.end())
		{
			if (itr->second->GetLightType() == TE_LIGHT_TYPE::Directional)
			{
				return static_cast<LightDirectional*>(itr->second);
			}
			else
			{
				TE_LOG_CORE_ERROR("The querried light was exist but the type was wrong!");
				return nullptr;
			}
		}

		return nullptr;
	}

	TruthEngine::Core::Camera* LightManager::GetLightCamera(const ILight* light)
	{
		auto itr = m_Map_LightsCamera.find(light->GetID());
		if (itr != m_Map_LightsCamera.end())
		{
			return itr->second;
		}

		return nullptr;
	}

	TruthEngine::Core::Camera* LightManager::AddLightCamera(const ILight* light, TE_CAMERA_TYPE cameraType)
	{
		auto cameraManager = CameraManager::GetInstance();

		Camera* camera = nullptr;

		switch (cameraType)
		{
		case TE_CAMERA_TYPE::Perspective:
			camera = cameraManager->CreatePerspectiveDirection(light->GetName().c_str(), light->GetPosition(), light->GetDirection(), float3{ .0f, 1.0f, .0f }, TE_PIDIV4, 1.0f, light->GetRange(), 1.0f);
			m_Map_LightsCamera[light->GetID()] = camera;
			break;
		case TE_CAMERA_TYPE::Orthographic:
			camera = cameraManager->CreateOrthographicCenterOff(light->GetName().c_str(), light->GetPosition(), light->GetDirection(), float3{ .0f, 1.0f, .0f }, -light->GetRange(), light->GetRange(), light->GetRange(), -light->GetRange(), light->GetRange(), 1.0f);
			m_Map_LightsCamera[light->GetID()] = camera;
			break;
		}

		return camera;
	}

	float4x4 LightManager::GetShadowTransform(const ILight* light)
	{
		if (auto camera = GetLightCamera(light); camera)
		{
			return camera->GetViewProj() * m_ProjToUV;
		}
		else 
		{
			return IdentityMatrix;
		}
	}

}