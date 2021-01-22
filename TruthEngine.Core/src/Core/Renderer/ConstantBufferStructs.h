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

	struct ConstantBuffer_Data_Per_DLight
	{
		ConstantBuffer_Data_Per_DLight() = default;
		ConstantBuffer_Data_Per_DLight(const float4& diffuse, const float4& ambient, const float4& specular, const float3& direction, float lightSize, const float3& position, uint32_t castShadow, float range, const float4x4 shadowTransform)
			: mDiffuse(diffuse), mAmbient(ambient), mSpecular(specular), mDirection(direction), mLightSize(lightSize), mPosition(position), mCastShadow(castShadow), mRange(range), mShadowTransform(shadowTransform)
		{}

		float4 mDiffuse;
		float4 mAmbient;
		float4 mSpecular;

		float3 mDirection;
		float mLightSize;

		float3 mPosition;
		uint32_t mCastShadow;

		float mRange;
		float3 _padLight;

		float4x4 mShadowTransform = IdentityMatrix;
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
