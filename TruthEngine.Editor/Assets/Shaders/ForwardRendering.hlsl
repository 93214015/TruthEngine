#include "Lights.hlsli"



/////////////////////////////////////////////////////////////////
//////////////// Constant Buffers
/////////////////////////////////////////////////////////////////

cbuffer CBPerMesh : register(b0)
{
    row_major matrix gWorld;
    row_major matrix gWorldInverseTranspose;
    
    uint materialIndex;
    float3 padPerMesh;
}

#define REGISTER_CBPerFrame b1
#include "CBPerFrame.hlsli"

#define REGISTER_CBLights b2
#include "CBLights.hlsli"

#define REGISTER_CBMaterials b3
#include "CBMaterials.hlsli"

#define REGISTER_CBUnfrequent b4
#include "CBUnfrequent.hlsli"

#define REGISTER_CBBoneTransforms b5
#include "CBBoneTransforms.hlsli"

///////////////////////////////////////////////////
//////////////// Textures
///////////////////////////////////////////////////
Texture2D<float> tShadowMap_SunLight : register(t0, space0);
Texture2D<float> tShadowMap_SpotLight : register(t1, space0);
TextureCube tEnvironmentMap : register(t2, space0);
TextureCube tIBLAmbient : register(t3, space0);
TextureCube tIBLSpecular : register(t4, space0);
Texture2D<float2> tPrecomputedBRDF : register(t5, space0);

Texture2D MaterialTextures[500] : register(t6, space0);


///////////////////////////////////////////////////
//////////////// Samplers
///////////////////////////////////////////////////
#include "Samplers.hlsli"


#ifdef MESH_TYPE_SKINNED

struct vertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
    float3 BoneWeights      : BONEWEIGHT;
    uint4 BoneIndex          : BONEINDEX;
};

#else

struct vertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
};

#endif

struct vertexOut
{
    float4 pos : SV_Position;
    float3 posW : POSITION0;
    //float4 posLight : POSITION1;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float2 texCoord : TEXCOORD;
};

vertexOut vs(vertexInput vin)
{
    vertexOut vout;
    
#ifdef MESH_TYPE_SKINNED

    float weightSum = vin.BoneWeights[0] + vin.BoneWeights[1] + vin.BoneWeights[2];

    float weight4 = 1.0 - weightSum;

    float4 pos = vin.BoneWeights[0] * mul(float4(vin.position, 1.0f), gBoneTransformations[vin.BoneIndex[0]]);
    pos += vin.BoneWeights[1] * mul(float4(vin.position, 1.0f), gBoneTransformations[vin.BoneIndex[1]]);
    pos += vin.BoneWeights[2] * mul(float4(vin.position, 1.0f), gBoneTransformations[vin.BoneIndex[2]]);
    pos += weight4 * mul(float4(vin.position, 1.0f), gBoneTransformations[vin.BoneIndex[3]]);
    pos.w = 1.0f;
    
#else

    float4 pos = float4(vin.position, 1.0f);
    
#endif
    
    float4 posW = mul(pos, gWorld);
    vout.pos = mul(posW, viewProj);
    vout.posW = posW.xyz;
    //vout.posLight = mul(posW, shadowTransform);
    vout.normalW = mul(vin.normal, (float3x3) gWorldInverseTranspose);
    vout.tangentW = mul(vin.tangent, (float3x3) gWorld);
    vout.texCoord = vin.texCoord;
    
    
    return vout;
}

float4 ps(vertexOut pin) : SV_Target
{

    float3 normal = normalize(pin.normalW);

    Material _material = MaterialArray[materialIndex];
    float2 _texUV = (pin.texCoord * _material.UVScale) + _material.UVTranslate;
    
#ifdef ENABLE_MAP_NORMAL
        float3 tangent = normalize(pin.tangentW);
        float3 bitangent = cross(normal, tangent);
        float3x3 TBN = float3x3(tangent, bitangent, normal);
    
    
        normal = MaterialTextures[_material.MapIndexNormal].Sample(sampler_point_wrap, _texUV).xyz;
        normal = (normal * 2.0f) - 1.0f;
        
        normal = normalize(mul(normal, TBN));
	
#endif
    
	
    //normal = mul(normal, (float3x3)gWorldInverseTranspose);
    
    float3 _MaterialAlbedo = _material.Diffuse.xyz;
    
#ifdef ENABLE_MAP_DIFFUSE
        _MaterialAlbedo = MaterialTextures[_material.MapIndexDiffuse].Sample(sampler_linear, _texUV).xyz;
#endif
	
    
#ifdef ENABLE_MAP_SPECULAR
    float3 _FresnelR0 = MaterialTextures[_material.MapIndexSpecular].Sample(sampler_point_wrap, _texUV).xxx;
#else
    float3 _FresnelR0 = _material.Metallic.xxx;
#endif
    
    float3 toEye = normalize(EyePos.xyz - pin.posW);

    float3 litColor = float3(.0f, .0f, .0f);
    
    float _Shininess = 1.0f - _material.Roughness;
    
    for (uint _DLightIndex = 0; _DLightIndex < gDLightCount; ++_DLightIndex)
    {
        
        float3 lit = ComputeDirectLight(gDLights[_DLightIndex], _MaterialAlbedo, _Shininess, _FresnelR0, pin.posW, normal, toEye);
		
        //float3 shadowMapCoords = pin.posLight.xyz / pin.posLight.w;

        /***
        
        //code for cascaded shadow map; finding corrsponding shadow map cascade and coords
        bool found = false;
        float3 shadowMapCoords = float3(0.0f, 0.0f, 0.0f);
    
        shadowMapCoords = mul(float4(pin.posW, 1.0f), gCascadedShadowTransform[0]).xyz;

        if (shadowMapCoords.x >= 0.0f && shadowMapCoords.y >= 0.0f && shadowMapCoords.x <= 0.5f && shadowMapCoords.y <= 0.5f)
        {
            found = true;
        }
    
        if (!found)
        {
            shadowMapCoords = mul(float4(pin.posW, 1.0f), gCascadedShadowTransform[1]).xyz;

            if (shadowMapCoords.x >= 0.5f && shadowMapCoords.y >= 0.0f && shadowMapCoords.x <= 1.0f && shadowMapCoords.y <= 0.5f)
            {
                found = true;
            }
        }
        if (!found)
        {
            shadowMapCoords = mul(float4(pin.posW, 1.0f), gCascadedShadowTransform[2]).xyz;

            if (shadowMapCoords.x >= 0.0f && shadowMapCoords.y >= 0.5f && shadowMapCoords.x <= 0.5f && shadowMapCoords.y <= 1.0f)
            {
                found = true;
            }
        }
        if (!found)
        {
            shadowMapCoords = mul(float4(pin.posW, 1.0f), gCascadedShadowTransform[3]).xyz;

            if (shadowMapCoords.x >= 0.5f && shadowMapCoords.y >= 0.5f && shadowMapCoords.x <= 1.0f && shadowMapCoords.y <= 1.0f)
            {
                found = true;
            }
        }

        float shadowMapSample = tShadowMap_SunLight.Sample(sampler_point_borderBlack, shadowMapCoords.xy);
        float shadowFactor = (float) (shadowMapSample > shadowMapCoords.z);
    
        //shadowMapSample = tShadowMap_SunLight.Sample(sampler_point_borderWhite, shadowMapCoords.xy, int2(0, 1));
		//shadowFactor += (float) (shadowMapSample < shadowMapCoords.z);
    
        //shadowMapSample = tShadowMap_SunLight.Sample(sampler_point_borderWhite, shadowMapCoords.xy, int2(1, 0));
		//shadowFactor += (float) (shadowMapSample < shadowMapCoords.z);
    
        //shadowMapSample = tShadowMap_SunLight.Sample(sampler_point_borderWhite, shadowMapCoords.xy, int2(1, 1));
		//shadowFactor += (float) (shadowMapSample < shadowMapCoords.z);
    
		//shadowFactor *= 0.25;
        //float shadowFactor = tShadowMap_SunLight.SampleCmp(samplerComparison_great_point_borderWhite, shadowMapCoords.xy, shadowMapCoords.z);
        
        //material_albedo = lightFactor * (material_albedo * Diffuse.xyz).xyz;
        
        litColor += lit * shadowFactor.xxx;

        ***/
        
        litColor += lit;
    }


    for (uint _SLightIndex = 0; _SLightIndex < gSLightCount; ++_SLightIndex)
    {
        float3 lit = ComputeSpotLight(gSpotLights[_SLightIndex], _MaterialAlbedo, _Shininess, _FresnelR0, pin.posW, normal, toEye);
		
        /***
        float4 shadowMapCoords = mul(float4(pin.posW, 1.0f), gSpotLights[_SLightIndex].ShadowTransform);
        shadowMapCoords.xyz /= shadowMapCoords.w;
				
        float shadowFactor = 0.0f;
		[unroll]
        for (int i = -2; i < 2; ++i)
        {
			[unroll]
            for (int j = -2; j <= 2; ++j)
            {
                float shadowMapSample = tShadowMap_SpotLight.Sample(sampler_point_borderWhite, shadowMapCoords.xy, int2(i, j));
                shadowFactor += (float) (shadowMapSample > shadowMapCoords.z);
            }
        }
    
        shadowFactor *= 0.04f;
        
        //float _ShadowFactor = tShadowMap_SpotLight.SampleCmp(samplerComparison_less_point_borderWhite, shadowMapCoords.xy, shadowMapCoords.z);

        litColor += lit * shadowFactor.xxx;
        
        ***/
        
        litColor += lit;

    }
    
    for (uint _PLightIndex = 0; _PLightIndex < gPLightCount; ++_PLightIndex)
    {
        float3 _Illumination = ComputePointLight(gPointLights[_PLightIndex], _MaterialAlbedo, _Shininess, _FresnelR0, pin.posW, normal, toEye);

        litColor += _Illumination;
    }
    
    if (gEnabledEnvironmentMap)
    {
        float3 reflectVector = reflect(-toEye, pin.normalW);
        float3 evironmentReflectColor = tEnvironmentMap.Sample(sampler_linear, reflectVector).xyz;
        float3 frenselFactor = SchlikFresnel(_FresnelR0, pin.normalW, reflectVector);
    
        litColor.rgb += _Shininess * frenselFactor * evironmentReflectColor;
    }
	
	//Add Global Ambient Light Factor
    litColor += (_MaterialAlbedo * gAmbientLightStrength);
    
#ifndef ENABLE_HDR
    litColor /= (litColor + 1.0f.xxx);
    //Gamma Correction
    litColor = pow(litColor, (1.0f / 2.2f).xxx);
#endif
    
    return float4(litColor, 1.0f);
}