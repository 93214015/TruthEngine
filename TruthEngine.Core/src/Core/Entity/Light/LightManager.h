#pragma once
#include "LightDirectional.h"
#include "LightSpot.h"
#include "LightPoint.h"

#include "Core/Entity/Camera/CameraCascadedFrustum.h"

namespace TruthEngine
{
	class ILight;
	class LightDirectional;

	class Camera;
	enum class TE_CAMERA_TYPE;

	class LightManager
	{
	public:

		LightManager();

		LightDirectional* AddLightDirectional(
			std::string_view name
			, const float3& position
			, float lightSize
			, const float3& strength
			, float strengthMultiplier
			, const float3& direction
			, uint32_t castShadow
			, const float4& CascadesCoveringDepth);

		LightSpot* AddLightSpot(
			std::string_view _Name,
			const float3& _Position,
			float _LightSize,
			const float3& _Strength,
			float _StrengthMultiplier,
			const float3& _Direction,
			bool _IsCastShadow,
			float _FalloffStart,
			float _FalloffEnd,
			float _InnerConeAngle,
			float _OuterConeAngle
		);

		LightPoint* AddLightPoint
		(
			std::string_view _Name,
			const float3& _Position,
			float _LightSize,
			const float3& _Strength,
			float _StrengthMultiplier,
			bool _CastShadow,
			float AttenuationStartRadius,
			float AttenuationEndRadius
			/*float _AttenuationConstant,
			float _AttenuationLinear,
			float _AttenuationQuadrant*/
		);


		LightDirectional* GetDirectionalLight(const std::string_view name)const;
		LightDirectional* GetDirectionalLight(uint32_t _LightID)const;
		LightSpot* GetSpotLight(const std::string_view name)const;
		LightSpot* GetSpotLight(uint32_t _LightID)const;

		inline size_t GetLightDirectionalCount() const noexcept
		{
			return m_LightsDirectional.size();
		}

		inline size_t GetLightSpotCount() const noexcept
		{
			return m_LightsSpot.size();
		}

		inline size_t GetLightPointCount() const noexcept
		{
			return m_LightsPoint.size();
		}

		Camera* GetLightCamera(const ILight* light);
		float4x4A GetShadowTransform(const ILight* light);
		void GetCascadedShadowTransform(const LightDirectional* light, float4x4A _outTransforms[4]);
		void GetCascadedShadowTransform(const CameraCascadedFrustumBase* _cameraCascaded, float4x4A _outTransforms[4]);
		Camera* AddLightCamera(const ILight* light, float _CameraViewRange, TE_CAMERA_TYPE cameraType);

		template<size_t cascadeNum>
		CameraCascadedFrustum<cascadeNum>* AddLightCameraCascaded(const char* name, const float3& position, const float3& direction, const float4& cascadeCoveringDepth, TE_CAMERA_TYPE cameraType);

		static LightManager* GetInstace()
		{
			static LightManager s_Instance;
			return &s_Instance;
		}


	protected:

	protected:
		std::unordered_map<uint32_t, ILight*> m_Map_Lights;
		std::unordered_map<std::string_view, ILight*> m_Map_LightsName;
		std::unordered_map<uint32_t, Camera*> m_Map_LightsCamera;

		std::vector<LightDirectional> m_LightsDirectional;
		std::vector<LightSpot> m_LightsSpot;
		std::vector<LightPoint> m_LightsPoint;
	};


	template<size_t cascadeNum>
	CameraCascadedFrustum<cascadeNum>* TruthEngine::LightManager::AddLightCameraCascaded(const char* name, const float3& position, const float3& direction, const float4& cascadeCoveringDepth, TE_CAMERA_TYPE cameraType)
	{
		auto cameraManager = CameraManager::GetInstance();

		CameraCascadedFrustum<cascadeNum>* camera = nullptr;

		switch (cameraType)
		{
		case TE_CAMERA_TYPE::Perspective:
			throw;
			break;
		case TE_CAMERA_TYPE::Orthographic:
		{
			camera = cameraManager->CreateOrthographicCascaded<cascadeNum>(name, &cascadeCoveringDepth.x, position, direction, float3{ .0f, 1.0f, .0f });
			break;
		}
		}

		return camera;
	}


}

#define TE_INSTANCE_LIGHTMANAGER TruthEngine::LightManager::GetInstace()