
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


struct CLightDirectionalData
{
    float4 Diffuse;
    float4 Ambient;
    float4 Specular;

    float3 Direction;
    float LightSize;
    
    bool CastShadow;
    float3 padDLight;
    
    //float3 Position;
    
    //float Range;
    //float3 padLight;
    
    //row_major matrix shadowTransform;
};

///////////////////////////////////////////////////
//////////////// Constant Buffers
///////////////////////////////////////////////////

cbuffer CBPerFrame : register(b0)
{
    row_major matrix viewProj;
    
    float3 EyePos;
    float pad;

    row_major matrix gCascadedShadowTransform[4];
}

cbuffer CBLightData : register(b1)
{
    CLightDirectionalData gDLights[1];
}

cbuffer CBMaterials : register(b2)
{
    Material MaterialArray[200];
}

cbuffer CBUnfrequent : register(b3)
{
    int gDLightCount;
    float3 padCBunfrequent;
}

cbuffer per_mesh : register(b4)
{
    row_major matrix gWorld;
    row_major matrix gWorldInverseTranspose;
    
    uint materialIndex;
    float3 padPerMesh;
}

///////////////////////////////////////////////////
//////////////// Textures
///////////////////////////////////////////////////
Texture2D<float> tShadowMap : register(t0, space0);
TextureCube tEnvironmentCubeMap : register(t1, space0);

Texture2D<float4> MaterialTextures[500] : register(t2, space0);


///////////////////////////////////////////////////
//////////////// Samplers
///////////////////////////////////////////////////
sampler sampler_linear : register(s0);
sampler sampler_point_borderBlack : register(s1);
sampler sampler_point_borderWhite : register(s2);
SamplerComparisonState samplerComparison_great_point_borderWhite : register(s3);


struct VertexInput
{
    float3 position : POSITION;
};

struct VertexOutput
{
    float4 PosH : SV_Position;
    float3 PosL : POSITION;
};


VertexOutput vs(VertexInput vin)
{
    VertexOutput vout;
    vout.PosL = vin.position;
        
    float3 posW = vin.position * float3(5000.0, 5000.0, 5000.0);
    posW += EyePos;
    vout.PosH = mul(float4(posW, 1.0f), viewProj).xyww;

    return vout;
}

float4 ps(VertexOutput pin) : SV_Target
{
    return tEnvironmentCubeMap.Sample(sampler_linear, pin.PosL);

}
