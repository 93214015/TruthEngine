#pragma once

#include "Core/Entity/Light/ILight.h"

namespace TruthEngine::Core {

	struct ConstantBuffer_Data_Per_Frame
	{
		DirectX::XMFLOAT4X4 m_VP;
		DirectX::XMFLOAT4 m_Color{ 0.6f, 0.6f, 0.6f, 1.0f };
	};

	struct ConstantBuffer_Data_Per_DLight : DirectionalLightData
	{
	};

	struct ConstantBuffer_Data_Materials
	{

		struct Material {
			Material() = default;
			Material(float4 diffuse, float4 ambient, float4 specular)
				: Diffuse(diffuse), Ambient(ambient), Specular(specular)
			{}

			float4 Diffuse;
			float4 Ambient;
			float4 Specular;
		};

		Material Materials[1000];
	};

}
