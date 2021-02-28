#pragma once
#include "ILight.h"

namespace TruthEngine
{
		class LightDirectional : public ILight
		{
		public:
			LightDirectional(
				uint32_t id
				, std::string_view name
				, const float4 diffusecolor
				, const float4 ambientColor
				, const float4 specularColor
				, const float3 direction
				, const float3 position
				, const float lightSize
				, const int castShadow
				, const float range);

			inline DirectionalLightData& GetDirectionalLightData() noexcept {
				return m_DLightData;
			}

		protected:

			DirectionalLightData m_DLightData;

		};
}
