#pragma once

#include "Core/Entity/Light/ILight.h"

namespace TruthEngine {

	struct alignas(16) ConstantBuffer_Data_Per_Frame
	{
		ConstantBuffer_Data_Per_Frame() = default;
		ConstantBuffer_Data_Per_Frame(const float4x4A& viewProj, const float4x4A& view, const float4x4A& viewInverse, const float4x4A& projection, const float4A& projectionValues, const float3& eyePos, const float4x4A cascadedShadowTransforms[4])
			: ViewProj(viewProj), View(view), ViewInverse(viewInverse), Projection(projection), ProjectionValues(projectionValues), EyePos(eyePos)
		{
			memcpy(CascadedShadowTransforms, cascadedShadowTransforms, 4 * sizeof(float4x4A));
		}

		float4x4A ViewProj;

		float4x4A View;

		float4x4A ViewInverse;

		float4x4A Projection;

		float4A ProjectionValues;

		float3 EyePos{0.0f, 0.0f, 0.0f};
		float pad = 0.0f;

		float4x4A CascadedShadowTransforms[4];
	};

	struct alignas(16) ConstantBuffer_Data_UnFrequent
	{
		ConstantBuffer_Data_UnFrequent()
			: mDLightCount(0), mSLightCount(0), mPLightCount(0) , mEnabledEnvironmentMap(1), mAmbientLightStrength(.5f, .5f, .5f)
		{}

		ConstantBuffer_Data_UnFrequent(
			uint32_t _LightDirectionalCount, uint32_t _LightSpotCount, uint32_t _LightPointCount
			, bool _EnabledEnvironmentMap, const float3& _AmbientLightStrength
			, const float2& _SceneViewportSize)
			: mDLightCount(_LightDirectionalCount), mSLightCount(_LightSpotCount), mPLightCount(_LightPointCount)
			, mEnabledEnvironmentMap(static_cast<uint32_t>(_EnabledEnvironmentMap)), mAmbientLightStrength(_AmbientLightStrength)
			, mSceneViewportSize(_SceneViewportSize), mSceneViewportStep(float2(1.0f /_SceneViewportSize.x, 1.0f / _SceneViewportSize.y))
		{}

		uint32_t mDLightCount;
		uint32_t mSLightCount;
		uint32_t mPLightCount;
		uint32_t mEnabledEnvironmentMap;

		float3 mAmbientLightStrength;
		float mPad1 = 0.0f;

		float2 mSceneViewportSize;
		float2 mSceneViewportStep;
	};

	struct alignas(16) ConstantBuffer_Struct_DirectionalLight
	{
		ConstantBuffer_Struct_DirectionalLight() = default;
		ConstantBuffer_Struct_DirectionalLight(const float3& position, float lightSize, const float3& strength, uint32_t castShadow, const float3& direction)
			: mPosition(position), mLightSize(lightSize), mStrength(strength), mCastShadow(castShadow), mDirection(direction)
		{}

		float3 mPosition;
		float mLightSize;

		float3 mStrength;
		bool mCastShadow;

		float3 mDirection;
		float Pad0;
	};

	struct alignas(16) ConstantBuffer_Struct_SpotLight
	{
		ConstantBuffer_Struct_SpotLight() = default;
		ConstantBuffer_Struct_SpotLight(
			const float4x4A& _ShadowTransform,
			const float3& _Position,
			const float _LightSize, 
			const float3& _Strength, 
			bool _IsCastShadow, 
			const float3& _Direction, 
			const float _FalloffStart,
			const float _FalloffEnd,
			const float _OuterConeAngleCos,
			const float _OuterConeAngleRangeRcp)
			: 
			mShadowTransform(_ShadowTransform),
			mPosition(_Position),
			mLightSize(_LightSize), 
			mStrength(_Strength), 
			mCastShadow(_IsCastShadow), 
			mDirection(_Direction), 
			mFalloffStart(_FalloffStart), 
			mFalloffEnd(_FalloffEnd), 
			mOuterConeCos(_OuterConeAngleCos), 
			mOuterConeAngleRangeCosRcp(_OuterConeAngleRangeRcp)
		{}

		float4x4A mShadowTransform;

		float3 mPosition;
		float mLightSize;

		float3 mStrength;
		uint32_t mCastShadow;

		float3 mDirection;
		float mFalloffStart;

		float mFalloffEnd;
		float mOuterConeCos;
		float mOuterConeAngleRangeCosRcp;
		float Pad0;
	};

	struct alignas(16) ConstantBuffer_Struct_PointLight
	{
		ConstantBuffer_Struct_PointLight() = default;
		ConstantBuffer_Struct_PointLight(const float3& _Position, float _LightSize, const float3& _Strength, bool _CastShadow, float _AttenuationStartRadius, float _AttenuationEndRadius)
			: Position(_Position), LightSize(_LightSize), Strength(_Strength), CastShadow(static_cast<uint32_t>(_CastShadow)), AttenuationStartRadius(_AttenuationStartRadius), AttenuationEndRadius(_AttenuationEndRadius)
		{}

		float3 Position;
		float LightSize;

		float3 Strength;
		uint32_t CastShadow;

		float AttenuationStartRadius;
		float AttenuationEndRadius;
		float2 _pad = float2{0.0f, 0.0f};

		/*float AttenuationConstant;
		float AttenuationLinear;
		float AttenuationQuadrant;
		float _Pad;*/
	};

	struct alignas(16) ConstantBuffer_Data_LightData
	{
		ConstantBuffer_Struct_DirectionalLight mDLights[1];
		ConstantBuffer_Struct_SpotLight mSLights[1];
		ConstantBuffer_Struct_PointLight mPLights[20];
	};

	struct alignas(16) ConstantBuffer_Data_Materials
	{
		struct Material {
			Material() = default;
			Material(
				const float4& diffuse
				, const float _Roughness
				, const float _Metallic
				, const float _AmbientOcclusion
				, const float _Emission
				, const float2& uvScale
				, const float2& uvTranslate 
				, uint32_t mapIndexDiffuse
				, uint32_t mapIndexNormal
				, uint32_t mapIndexDisplacement
				, uint32_t mapIndexSpecular
				, uint32_t mapIndexRoughness
				, uint32_t mapIndexMetallic
				, uint32_t mapIndexAmbientOcclusion
				, uint32_t enabledSSR
			)
				: Diffuse(diffuse), Roughness(_Roughness), Metallic(_Metallic), AmbientOcclusion(_AmbientOcclusion), Emission(_Emission)
				, MapIndexDiffuse(mapIndexDiffuse), MapIndexNormal(mapIndexNormal), MapIndexDisplacement(mapIndexDisplacement), MapIndexSpecular(mapIndexSpecular)
				, MapIndexRoughness(mapIndexRoughness), MapIndexMetallic(mapIndexMetallic), MapIndexAmbientOcclusion(mapIndexAmbientOcclusion)
				, UVScale(uvScale), UVTranslate(uvTranslate), EnabledSSR(enabledSSR)
			{}

			float4 Diffuse = float4{ .0f,.0f,.0f,1.0f };

			float Roughness = .5f;
			float Metallic = .0f;
			float AmbientOcclusion = 1.0f;
			float Emission = .0f;
			//float3 FresnelR0 = float3{ .0f,.0f,.0f };
			//float Shininess = .0f;

			float2 UVScale = float2{ 1.0f, 1.0f };
			float2 UVTranslate = float2{ .0f,.0f };

			uint32_t MapIndexDiffuse = -1;
			uint32_t MapIndexNormal = -1;
			uint32_t MapIndexDisplacement = -1;
			uint32_t MapIndexSpecular = -1;

			uint32_t MapIndexRoughness = -1;
			uint32_t MapIndexMetallic = -1;
			uint32_t MapIndexAmbientOcclusion = -1;
			uint32_t EnabledSSR = 0;
			
		};

		Material MaterialArray[100];
	};

	struct alignas(16) ConstantBuffer_Data_Bones
	{
		float4x4A mBones[256];
	};

	struct alignas(16) ConstantBuffer_Data_EnvironmentMap
	{
		ConstantBuffer_Data_EnvironmentMap()
			: mEnvironmentMapMultiplier(float3(1.0f, 1.0f, 1.0f))
		{}

		ConstantBuffer_Data_EnvironmentMap(const float3& _EnvironmentMapMultiplier)
			: mEnvironmentMapMultiplier(_EnvironmentMapMultiplier)
		{}

		float3 mEnvironmentMapMultiplier;
		float _Pad = 0.0f;
	};

}
