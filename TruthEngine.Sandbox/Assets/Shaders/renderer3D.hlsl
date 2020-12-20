
cbuffer per_frame : register(b0)
{
    matrix viewProj : packoffset(c0);
    float4 color : packoffset(c4.x);
}

struct vertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord :TEXCOORD;
};

float4 vs(vertexInput vin) : SV_POSITION
{	
    return float4(vin.position, 1.0f);
}

float4 ps( float4 pos : SV_POSITION ) : SV_Target
{
    return color;
}