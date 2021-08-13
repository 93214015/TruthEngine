#include "LightsPBR.hlsli"



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

Texture2D MaterialTextures[500] : register(t3, space0);


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
        //normal = normalize(normal);
        
        normal = normalize(mul(normal, TBN));
	
		//float normalLength = length(normal);
#endif   

    
#ifdef ENABLE_MAP_DIFFUSE
    float3 _MaterialAlbedo = MaterialTextures[_material.MapIndexDiffuse].Sample(sampler_linear, _texUV).xyz;
#else
    float3 _MaterialAlbedo = _material.Diffuse.xyz;
#endif
	
    
#ifdef ENABLE_MAP_ROUGHNESS
    float _Roughness = MaterialTextures[_material.MapIndexRoughness].Sample(sampler_point_wrap, _texUV).r;
#else
    float _Roughness = _material.Roughness;
#endif
    
#ifdef ENABLE_MAP_METALLIC
    float _Metallic = MaterialTextures[_material.MapIndexMetallic].Sample(sampler_point_wrap, _texUV).r;
#else
    float _Metallic = _material.Metallic;
#endif
    
#ifdef ENABLE_MAP_AMBIENTOCCLUSION
    float _AmbientOcclusion = MaterialTextures[_material.MapIndexAmbientOcclusion].Sample(sampler_point_wrap, _texUV).r;
#else
    float _AmbientOcclusion = _material.AmbientOcclusion;
#endif
    
    float3 toEye = normalize(EyePos.xyz - pin.posW);

    float3 litColor = float3(.0f, .0f, .0f);
    
    float3 _F0 = float3(0.4f, 0.4f, 0.4f);
    _F0 = lerp(_F0, _MaterialAlbedo, _Metallic);
    
    for (uint _DLightIndex = 0; _DLightIndex < gDLightCount; ++_DLightIndex)
    {
        
        float3 lit = ComputeDirectLight(gDLights[_DLightIndex], _MaterialAlbedo, _Roughness, _Metallic, _F0, normal, toEye);
        
        litColor += lit;
    }


    for (uint _SLightIndex = 0; _SLightIndex < gSLightCount; ++_SLightIndex)
    {
        float3 lit = ComputeSpotLight(gSpotLights[_SLightIndex], _MaterialAlbedo, _Roughness, _Metallic, _F0, pin.posW, normal, toEye);

        litColor += lit;
    }
	
	//Add Global Ambient Light Factor
    litColor += (_MaterialAlbedo * gAmbientLightStrength * _AmbientOcclusion.xxx);
    litColor /= litColor + float3(1.0f, 1.0f, 1.0f);
    litColor = pow(litColor, (1.0f / 2.2f).xxx);
    
    return float4(litColor, 1.0f);
}