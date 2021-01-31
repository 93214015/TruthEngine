#pragma once
#include "LightDirectional.h"

namespace TruthEngine
{
	namespace Core
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

			LightDirectional* GetDirectionalLight(const std::string_view name);

			Camera* GetLightCamera(const ILight* light);
			float4x4 GetShadowTransform(const ILight* light);
			Camera* AddLightCamera(const ILight* light, TE_CAMERA_TYPE cameraType);

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
		};
	}
}

