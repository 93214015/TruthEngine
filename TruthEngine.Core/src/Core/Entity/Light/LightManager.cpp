#include "pch.h"
#include "LightManager.h"

#include "LightDirectional.h"

#include "Core/Entity/Camera/CameraManager.h"
#include "Core/Application.h"
#include "Core/Event/EventEntity.h"

constexpr float4x4 m_ProjToUV = float4x4(0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.5, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 1.0f);

constexpr float4x4 m_ProjToUVCascaded0 = float4x4(
	0.25f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.25, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.25f, 0.25f, 0.0f, 1.0f);

constexpr float4x4 m_ProjToUVCascaded1 = float4x4(
	0.25f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.25, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.75f, 0.25f, 0.0f, 1.0f);

constexpr float4x4 m_ProjToUVCascaded2 = float4x4(
	0.25f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.25, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.25f, 0.75f, 0.0f, 1.0f);

constexpr float4x4 m_ProjToUVCascaded3 = float4x4(
	0.25f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.25, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.75f, 0.75f, 0.0f, 1.0f);

namespace TruthEngine
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

		EventEntityAddLight _Event(dlight);
		TE_INSTANCE_APPLICATION->OnEvent(_Event);
		
		return dlight;
	}

	LightDirectional* LightManager::GetDirectionalLight(const std::string_view name) const
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

	TruthEngine::LightDirectional* LightManager::GetDirectionalLight(uint32_t _LightID) const
	{
		auto itr = m_Map_Lights.find(_LightID);
		if (itr != m_Map_Lights.end())
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

	TruthEngine::Camera* LightManager::GetLightCamera(const ILight* light)
	{
		auto itr = m_Map_LightsCamera.find(light->GetID());
		if (itr != m_Map_LightsCamera.end())
		{
			return itr->second;
		}

		return nullptr;
	}


	TruthEngine::CameraCascadedFrustumBase* LightManager::GetLightCameraCascaded(const ILight* light)
	{
		auto itr = m_Map_LightsCameraCascaded.find(light->GetID());
		if (itr != m_Map_LightsCameraCascaded.end())
		{
			return itr->second;
		}

		return nullptr;
	}


	TruthEngine::Camera* LightManager::AddLightCamera(const ILight* light, TE_CAMERA_TYPE cameraType)
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

	void LightManager::GetCascadedShadowTransform(const ILight* light, float4x4 _outTransforms[4])
	{

		auto itr = m_Map_LightsCameraCascaded.find(light->GetID());
		if (itr == m_Map_LightsCameraCascaded.end())
		{
			TE_ASSERT_CORE(false, "Cascaded Camera Not Found!");
			throw;
		}

		auto camera = itr->second;

		_outTransforms[0] = camera->GetViewProj(0) * m_ProjToUVCascaded0;
		_outTransforms[1] = camera->GetViewProj(1) * m_ProjToUVCascaded1;
		_outTransforms[2] = camera->GetViewProj(2) * m_ProjToUVCascaded2;
		_outTransforms[3] = camera->GetViewProj(3) * m_ProjToUVCascaded3;
		
	}

	void LightManager::GetCascadedShadowTransform(const CameraCascadedFrustumBase* _cameraCascaded, float4x4 _outTransforms[4])
	{
		_outTransforms[0] = _cameraCascaded->GetViewProj(0) * m_ProjToUVCascaded0;
		_outTransforms[1] = _cameraCascaded->GetViewProj(1) * m_ProjToUVCascaded1;
		_outTransforms[2] = _cameraCascaded->GetViewProj(2) * m_ProjToUVCascaded2;
		_outTransforms[3] = _cameraCascaded->GetViewProj(3) * m_ProjToUVCascaded3;
	}

}