#pragma once

#include "Core/Entity/Light/ILight.h"

namespace TruthEngine::Core {

	struct ConstantBuffer_Data_Per_Frame
	{
		ConstantBuffer_Data_Per_Frame(float4x4 viewProj, float3 eyePos)
			: ViewProj(viewProj), EyePos(eyePos)
		{}

		float4x4 ViewProj;
		float3 EyePos{0.0f, 0.0f, 0.0f};
		float pad = 0.0f;
	};

	struct ConstantBuffer_Data_Per_DLight : DirectionalLightData
	{
	};

	struct ConstantBuffer_Data_Materials
	{
		struct Material {
			Material() = default;
			Material(float4 diffuse, float3 fresnelR0, float shininess , uint32_t mapIndexDiffuse, uint32_t mapIndexNormal, uint32_t mapIndexDisplacement)
				: Diffuse(diffuse), FresnelR0(fresnelR0), Shininess(shininess)
				, MapIndexDiffuse(mapIndexDiffuse), MapIndexNormal(mapIndexNormal), MapIndexDisplacement(mapIndexDisplacement)
			{}

			float4 Diffuse;
			float3 FresnelR0;
			float Shininess;

			uint32_t MapIndexDiffuse;
			uint32_t MapIndexNormal;
			uint32_t MapIndexDisplacement;
			uint32_t pad;
		};

		Material MaterialArray[100];
	};

}
