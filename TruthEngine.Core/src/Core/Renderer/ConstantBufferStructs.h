#pragma once

#include "Core/Entity/Light/ILight.h"

namespace TruthEngine::Core {

	struct ConstantBuffer_Data_Per_Frame
	{
		ConstantBuffer_Data_Per_Frame(const float4x4& viewProj, const float3& eyePos)
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
			Material(const float4& diffuse, const float3& fresnelR0, float shininess, const float2& uvScale, const float2& uvTranslate , uint32_t mapIndexDiffuse, uint32_t mapIndexNormal, uint32_t mapIndexDisplacement)
				: Diffuse(diffuse), FresnelR0(fresnelR0), Shininess(shininess)
				, MapIndexDiffuse(mapIndexDiffuse), MapIndexNormal(mapIndexNormal), MapIndexDisplacement(mapIndexDisplacement)
				, UVScale(uvScale), UVTranslate(uvTranslate)
			{}

			float4 Diffuse = float4{ .0f,.0f,.0f,1.0f };
			float3 FresnelR0 = float3{ .0f,.0f,.0f };
			float Shininess = .0f;

			float2 UVScale = float2{ 1.0f, 1.0f };
			float2 UVTranslate = float2{ .0f,.0f };

			uint32_t MapIndexDiffuse = -1;
			uint32_t MapIndexNormal = -1;
			uint32_t MapIndexDisplacement = -1;
			uint32_t pad = 0;
		};

		Material MaterialArray[100];
	};

}
