

//
//ConstantBuffers
//
struct Material
{
    float4 Diffuse;
    float4 Ambient;
    float4 Specular;
    
    uint MapIndexDiffuse;
    uint MapIndexNormal;
    uint MapIndexDisplacement;
    uint pad;
};

cbuffer per_frame : register(b0)
{
    row_major matrix viewProj : packoffset(c0);
    float4 color : packoffset(c4.x);
}

cbuffer per_dlight : register(b1)
{
    float4x4 View;
    float4x4 ViewProj;
    float4x4 ShadowTransform;

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
    int IndexShadowMapSRV;
    float pad;
    
    float4 pad2[12];
}

cbuffer materials : register(b2)
{
    Material MaterialArray[100];
}

cbuffer per_mesh : register(b3)
{
    uint materialIndex;
}

///////////////////////////////////////////////////
//////////////// Textures
///////////////////////////////////////////////////
Texture2D<float4> MaterialTextures_Diffuse[50] : register(t0, space0);
Texture2D<float4> MaterialTextures_Normal[50] : register(t0, space1);


///////////////////////////////////////////////////
//////////////// Samplers
///////////////////////////////////////////////////
sampler sampler_point : register(s0);


struct vertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
    
};

struct vertexOut
{
    float4 pos : SV_Position;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float2 texCoord : TEXCOORD;
};

vertexOut vs(vertexInput vin)
{
    vertexOut vout;
    vout.pos = mul(float4(vin.position, 1.0f), viewProj);
    vout.normalW = vin.normal;
    vout.tangentW = vin.tangent;
    vout.texCoord = vin.texCoord;
    
    return vout;
}

float4 ps(vertexOut pin) : SV_Target
{

    float3 normal = normalize(pin.normalW);

    float3 tangent = normalize(pin.tangentW);
    
    float3 bitangent = cross(normal, tangent);
    
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    
    [branch]
    if (MaterialArray[materialIndex].MapIndexNormal != -1)
    {
        normal = MaterialTextures_Normal[MaterialArray[materialIndex].MapIndexNormal].Sample(sampler_point, pin.texCoord).xyz;
        normal = (normal * 2.0f) - 1.0f;
        normal = normalize(normal);
        
        normal = mul(normal, TBN);
    }
    
    float3 color = MaterialArray[materialIndex].Diffuse.xyz;
    
    [branch]
    if (MaterialArray[materialIndex].MapIndexDiffuse != -1)
    {
        color = MaterialTextures_Diffuse[MaterialArray[materialIndex].MapIndexDiffuse].Sample(sampler_point, pin.texCoord).xyz;
    }
    
    float3 lightVector = -1.0 * normalize(Direction);
    float dotResult = dot(lightVector, normal);
    float lightFactor = clamp(dotResult, 0.0f, 1.0f);
    
    color = lightFactor * (color * Diffuse.xyz).xyz;
    return float4(color, 1.0f);
}