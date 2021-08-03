//
//Data Structs
//
struct Material
{
    float4 Diffuse;
    
    float3 FresnelR0;
    float Shininess;
    
    float2 UVScale;
    float2 UVTranslate;
    
    uint MapIndexDiffuse;
    uint MapIndexNormal;
    uint MapIndexDisplacement;
    uint pad;
};


///////////////////////////////////////////////////
//////////////// Constant Buffers
///////////////////////////////////////////////////

cbuffer per_mesh : register(b0)
{
    row_major matrix gWorld;
    row_major matrix gWorldInverseTranspose;
    
    uint materialIndex;
    float3 padPerMesh;
}

cbuffer CBPerFrame : register(b1)
{
    row_major matrix viewProj;
    
    float3 EyePos;
    float pad;

    row_major matrix gCascadedShadowTransform[4];
}

cbuffer CBMaterials : register(b2)
{
    Material MaterialArray[200];
}

cbuffer cb_boneTransforms : register(b3)
{
    row_major matrix gBoneTransformations[256];
};

///////////////////////////////////////////////////
//////////////// Textures
///////////////////////////////////////////////////
TextureCube tEnvironmentMap : register(t2, space0);

Texture2D MaterialTextures[500] : register(t3, space0);


///////////////////////////////////////////////////
//////////////// Samplers
///////////////////////////////////////////////////
sampler sampler_linear : register(s0);
sampler sampler_point_borderBlack : register(s1);
sampler sampler_point_borderWhite : register(s2);
SamplerComparisonState samplerComparison_great_point_borderBlack : register(s3);
sampler sampler_point_wrap : register(s4);
SamplerComparisonState samplerComparison_less_point_borderWhite : register(s5);


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


struct PixelOut
{    
    float4 Color : SV_Target0;
    float3 Normal : SV_Target1;
};


PixelOut ps(vertexOut pin)
{
    
    PixelOut _PixelOut;
    
    _PixelOut.Normal = normalize(pin.normalW);

    Material _material = MaterialArray[materialIndex];
    float2 _texUV = (pin.texCoord * _material.UVScale) + _material.UVTranslate;
    
#ifdef ENABLE_MAP_NORMAL
        float3 tangent = normalize(pin.tangentW);
        float3 bitangent = cross(normal, tangent);
        float3x3 TBN = float3x3(tangent, bitangent, _PixelOut.Normal);
    
    
        _PixelOut.Normal = MaterialTextures[_material.MapIndexNormal].Sample(sampler_point_wrap, _texUV).xyz;
        _PixelOut.Normal = (_PixelOut.Normal * 2.0f) - 1.0f;
        
        _PixelOut.Normal = mul(_PixelOut.Normal, TBN);
	
		//float normalLength = length(normal);
	
#endif
    
    _PixelOut.Normal = normalize(_PixelOut.Normal);
    
    _PixelOut.Color = _material.Diffuse.xyz;
    
#ifdef ENABLE_MAP_DIFFUSE
        _PixelOut.Color = MaterialTextures[_material.MapIndexDiffuse].Sample(sampler_linear, _texUV).xyz;
#endif
    
    
    
    return _PixelOut;
}