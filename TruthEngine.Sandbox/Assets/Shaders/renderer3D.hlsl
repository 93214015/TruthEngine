
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

cbuffer per_mesh : register(b2)
{
    float4 colorMesh;
}

struct vertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord :TEXCOORD;
};

float4 vs(vertexInput vin) : SV_POSITION
{	
    
    return mul(float4(vin.position, 1.0f), viewProj);
}

float4 ps( float4 pos : SV_POSITION ) : SV_Target
{
    return colorMesh;
}