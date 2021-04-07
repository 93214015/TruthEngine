#include "pch.h"
#include "LightManager.h"

#include "LightDirectional.h"

#include "Core/Entity/Camera/CameraManager.h"
#include "Core/Application.h"
#include "Core/Event/EventEntity.h"




namespace TruthEngine
{

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


	LightDirectional* LightManager::AddLightDirectional(
		std::string_view name,
		const float3& strength,
		const float3& direction, 
		const float3& position, 
		float lightSize, 
		uint32_t castShadow, 
		const float4& CascadesCoveringDepth)
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

		std::string _CameraName = "camera" + std::string(name);

		auto _CascadedCamera = AddLightCameraCascaded<4>(_CameraName.c_str(), position, direction, CascadesCoveringDepth, TE_CAMERA_TYPE::Orthographic);

		auto dlight = &m_LightsDirectional.emplace_back(_id, name, strength, direction, position, lightSize, castShadow, CascadesCoveringDepth, _CascadedCamera);
		m_Map_Lights[_id] = dlight;
		m_Map_LightsName[name] = dlight;

		EventEntityAddLight _Event(dlight);
		TE_INSTANCE_APPLICATION->OnEvent(_Event);
		
		return dlight;
	}

	LightSpot* LightManager::AddLightSpot(
		std::string_view _Name, 
		const float3& _Strength, 
		const float3& _Direction, 
		const float3& _Position, 
		float _LightSize, 
		bool _IsCastShadow, 
		float _FalloffStart, 
		float _FalloffEnd, 
		float _InnerConeAngle, 
		float _OuterConeAngle)
	{
		uint32_t _id = m_Map_Lights.size();

		auto slight = &m_LightsSpot.emplace_back(_id, _Name, _Strength, _LightSize, _Direction, _IsCastShadow, _Position, _FalloffStart, _FalloffEnd, _InnerConeAngle, _OuterConeAngle);
		m_Map_Lights[_id] = slight;
		m_Map_LightsName[_Name] = slight;

		if (_IsCastShadow)
		{
			Camera* _Camera = AddLightCamera(slight, 0.0f, TE_CAMERA_TYPE::Perspective);
			slight->SetCamera(_Camera);
		}

		EventEntityAddLight _Event(slight);
		TE_INSTANCE_APPLICATION->OnEvent(_Event);

		return slight;
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

	LightSpot* LightManager::GetSpotLight(const std::string_view name) const
	{
		auto itr = m_Map_LightsName.find(name);
		if (itr != m_Map_LightsName.end())
		{
			if (itr->second->GetLightType() == TE_LIGHT_TYPE::Spot)
			{
				return static_cast<LightSpot*>(itr->second);
			}
			else
			{
				TE_LOG_CORE_ERROR("The querried light was exist but the type was wrong!");
				return nullptr;
			}
		}

		return nullptr;
	}

	LightSpot* LightManager::GetSpotLight(uint32_t _LightID) const
	{
		auto itr = m_Map_Lights.find(_LightID);
		if (itr != m_Map_Lights.end())
		{
			if (itr->second->GetLightType() == TE_LIGHT_TYPE::Spot)
			{
				return static_cast<LightSpot*>(itr->second);
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


	TruthEngine::Camera* LightManager::AddLightCamera(const ILight* light, float _CameraViewRange, TE_CAMERA_TYPE cameraType)
	{
		auto cameraManager = CameraManager::GetInstance();

		Camera* camera = nullptr;

		float _FOV = Math::PIDIV4;

		if (light->GetLightType() == TE_LIGHT_TYPE::Spot)
		{
			const LightSpot* _LightSpot = static_cast<const LightSpot*>(light);
			_FOV = _LightSpot->GetOuterConeAngle();
			_CameraViewRange = _LightSpot->GetFalloffEnd();
		}

		switch (cameraType)
		{
		case TE_CAMERA_TYPE::Perspective:
			camera = cameraManager->CreatePerspectiveDirection(light->GetName().c_str(), light->GetPosition(), light->GetDirection(), float3{ .0f, 1.0f, .0f }, _FOV, 1.0f, 1.0f, _CameraViewRange, false);
			m_Map_LightsCamera[light->GetID()] = camera;
			break;
		case TE_CAMERA_TYPE::Orthographic:
			camera = cameraManager->CreateOrthographicCenterOff(light->GetName().c_str(), light->GetPosition(), light->GetDirection(), float3{ .0f, 1.0f, .0f }, -1.0f * _CameraViewRange, _CameraViewRange, _CameraViewRange, -1.0f * _CameraViewRange, 1.0f, _CameraViewRange);
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

	void LightManager::GetCascadedShadowTransform(const LightDirectional* light, float4x4 _outTransforms[4])
	{
		auto _Camera = light->GetCamera();
		_outTransforms[0] = _Camera->GetViewProj(0) * m_ProjToUVCascaded0;
		_outTransforms[1] = _Camera->GetViewProj(1) * m_ProjToUVCascaded1;
		_outTransforms[2] = _Camera->GetViewProj(2) * m_ProjToUVCascaded2;
		_outTransforms[3] = _Camera->GetViewProj(3) * m_ProjToUVCascaded3;
		
	}

	void LightManager::GetCascadedShadowTransform(const CameraCascadedFrustumBase* _cameraCascaded, float4x4 _outTransforms[4])
	{
		_outTransforms[0] = _cameraCascaded->GetViewProj(0) * m_ProjToUVCascaded0;
		_outTransforms[1] = _cameraCascaded->GetViewProj(1) * m_ProjToUVCascaded1;
		_outTransforms[2] = _cameraCascaded->GetViewProj(2) * m_ProjToUVCascaded2;
		_outTransforms[3] = _cameraCascaded->GetViewProj(3) * m_ProjToUVCascaded3;
	}

}