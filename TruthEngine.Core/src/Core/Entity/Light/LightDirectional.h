#pragma once
#include "ILight.h"

namespace TruthEngine
{


	namespace Core 
	{

		class LightDirectional : public ILight
		{
		public:
			LightDirectional();

			LightDirectional(
				const std::string_view name
				, const float4 diffusecolor
				, const float4 ambientColor
				, const float4 specularColor
				, const float3 direction
				, const float3 position
				, const float lightSize
				, const int castShadow);

			inline DirectionalLightData& GetDirectionalLightData() noexcept {
				return m_DLightData;
			}

		protected:

			DirectionalLightData m_DLightData;

		};

	}
}
