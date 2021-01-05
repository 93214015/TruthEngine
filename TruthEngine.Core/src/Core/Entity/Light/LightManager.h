#pragma once

namespace TruthEngine
{
	namespace Core
	{
		class ILight;
		class LightDirectional;

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
				, const int castShadow);

			LightDirectional* GetDirectionalLight(const std::string_view name);

			static LightManager* GetInstace()
			{
				static LightManager s_Instance;
				return &s_Instance;
			}
		protected:

		protected:
			std::unordered_map<std::string_view, std::shared_ptr<LightDirectional>> m_Map_DirectionalLights;
		};
	}
}

