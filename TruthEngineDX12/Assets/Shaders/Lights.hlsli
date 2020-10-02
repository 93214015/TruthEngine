#ifndef HLSL_LIGHTS

#define HLSL_LIGHTS

#include "CommonSamplers.hlsli"


//
// Light Data Structures
//

struct DirLight
{
    matrix View;
    matrix ViewProj;
    matrix ShadowTransform;

    float4 PerpectiveValues;

    float4 Diffuse;
    float4 Ambient;
    float4 Specular;

    float3 Direction;
    float LightSize;

    float3 Position;
    float zNear;

    float2 DirInEyeScreen_Horz;
    float2 DirInEyeScreen_Vert;

    float zFar;
    bool CastShadow;
    int ShadowMapIndex;
    float pad;
};


struct SpotLight
{
    matrix View;
    matrix ViewProj;
    matrix ShadowTransform;

    float4 PerpectiveValues;

    float4 Diffuse;
    float4 Ambient;
    float4 Specular;

    float3 Direction;
    float LightSize;

    float3 Position;
    float zNear;

    float2 DirInEyeScreen_Horz;
    float2 DirInEyeScreen_Vert;

    float zFar;
    bool CastShadow;
    int ShadowMapIndex;
    float pad0;
    
    float3 Attenuation;
    float Range;

    float Spot;
    float3 pad1;
};


//
// Light Helpers Functions
//



//
// Shadow Helper Functions
//

float CalcShadowFactor_BasicShadows(float4 shadowPos, Texture2D<float> tShadowMap)
{
    
    float2 uv = shadowPos.xy;
    float z = shadowPos.z;
        
    float sampleDepth = tShadowMap.Sample(samplerPoint, uv).x;
    
    return step(z, sampleDepth);
}

#endif