#pragma once
#include "LightDirectional.h"

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

			LightDirectional* AddLightDirectional(const std::string_view name
				, const float4 diffusecolor
				, const float4 ambientColor
				, const float4 specularColor
				, const float3 direction
				, const float3 position
				, const float lightSize
				, const int castShadow
				, const float range);

			LightDirectional* GetDirectionalLight(const std::string_view name)const;
			LightDirectional* GetDirectionalLight(uint32_t _LightID)const;

			inline uint32_t GetLightDirectionalCount() const noexcept
			{
				return static_cast<uint32_t>(m_LightsDirectional.size());
			}

			Camera* GetLightCamera(const ILight* light);
			CameraCascadedFrustumBase* GetLightCameraCascaded(const ILight* light);
			float4x4 GetShadowTransform(const ILight* light);
			void GetCascadedShadowTransform(const ILight* light, float4x4 _outTransforms[4]);
			void GetCascadedShadowTransform(const CameraCascadedFrustumBase* _cameraCascaded, float4x4 _outTransforms[4]);
			Camera* AddLightCamera(const ILight* light, TE_CAMERA_TYPE cameraType);

			template<uint32_t cascadeNum>
			CameraCascadedFrustum<cascadeNum>* AddLightCameraCascaded(const ILight* light, const float cascadeDepths[cascadeNum], TE_CAMERA_TYPE cameraType);

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
			std::unordered_map<uint32_t, CameraCascadedFrustumBase*> m_Map_LightsCameraCascaded;

			std::vector<LightDirectional> m_LightsDirectional;
		};


		template<uint32_t cascadeNum>
		CameraCascadedFrustum<cascadeNum>* TruthEngine::LightManager::AddLightCameraCascaded(const ILight* light, const float cascadeDepths[cascadeNum], TE_CAMERA_TYPE cameraType)
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
				camera = cameraManager->CreateOrthographicCascaded<cascadeNum>(light->GetName().c_str(), cascadeDepths, light->GetPosition(), light->GetDirection(), float3{ .0f, 1.0f, .0f });
				m_Map_LightsCameraCascaded[light->GetID()] = camera;
				break;
			}
			}

			return camera;
		}


}

#define TE_INSTANCE_LIGHTMANAGER TruthEngine::LightManager::GetInstace()