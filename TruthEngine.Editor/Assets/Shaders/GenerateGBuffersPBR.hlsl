

///////////////////////////////////////////////////
//////////////// Constant Buffers
///////////////////////////////////////////////////

cbuffer CBPerMesh : register(b0)
{
    row_major matrix gWorld;
    row_major matrix gWorldInverseTranspose;
    
    uint materialIndex;
    float3 padPerMesh;
}

#define REGISTER_CBPerFrame b1
#include "CBPerFrame.hlsli"

#define REGISTER_CBMaterials b2
#include "CBMaterials.hlsli"

#define REGISTER_CBBoneTransforms b3
#include "CBBoneTransforms.hlsli"

///////////////////////////////////////////////////
//////////////// Textures
///////////////////////////////////////////////////
Texture2D MaterialTextures[500] : register(t0, space0);


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
    float3 posV : POSITION1;
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
    vout.posV = mul(posW, View).xyz;
    vout.pos = mul(float4(vout.posV, 1.0f), Projection);
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
    float4 Specular : SV_Target2;
    float4 PosView : SV_Target3;
};


PixelOut ps(vertexOut pin)
{
    
    PixelOut _PixelOut;
    
    _PixelOut.Normal = normalize(pin.normalW);

    Material _material = MaterialArray[materialIndex];
    float2 _texUV = (pin.texCoord * _material.UVScale) + _material.UVTranslate;
    
#ifdef ENABLE_MAP_NORMAL
        float3 tangent = normalize(pin.tangentW);
        float3 bitangent = cross(_PixelOut.Normal, tangent);
        float3x3 TBN = float3x3(tangent, bitangent, _PixelOut.Normal);
    
    
        _PixelOut.Normal = MaterialTextures[_material.MapIndexNormal].Sample(sampler_point_wrap, _texUV).xyz;
        _PixelOut.Normal = (_PixelOut.Normal * 2.0f) - 1.0f;
        
        _PixelOut.Normal = mul(_PixelOut.Normal, TBN);
        _PixelOut.Normal = normalize(_PixelOut.Normal);
	
#endif
    
    _PixelOut.Normal *= .5f;
    _PixelOut.Normal += .5f;
        
#ifdef ENABLE_MAP_DIFFUSE
    _PixelOut.Color = float4(MaterialTextures[_material.MapIndexDiffuse].Sample(sampler_anisotropic16x, _texUV).xyz, _material.Emission);
#else
    _PixelOut.Color = float4(_material.Diffuse.xyz, _material.Emission);
#endif
	
    
#ifdef ENABLE_MAP_ROUGHNESS
     _PixelOut.Specular.x = MaterialTextures[_material.MapIndexRoughness].Sample(sampler_point_wrap, _texUV).r;
#else
    _PixelOut.Specular.x = _material.Roughness;
#endif
    
#ifdef ENABLE_MAP_METALLIC
     _PixelOut.Specular.y = MaterialTextures[_material.MapIndexMetallic].Sample(sampler_point_wrap, _texUV).r;
#else
    _PixelOut.Specular.y = _material.Metallic;
#endif
    
#ifdef ENABLE_MAP_AMBIENTOCCLUSION
     _PixelOut.Specular.z = MaterialTextures[_material.MapIndexAmbientOcclusion].Sample(sampler_point_wrap, _texUV).r;
#else
    _PixelOut.Specular.z = _material.AmbientOcclusion;
#endif
    
    _PixelOut.Specular.w = 1.0f;
    
    _PixelOut.PosView = float4(pin.posV, 1.0f);
    
    return _PixelOut;
}