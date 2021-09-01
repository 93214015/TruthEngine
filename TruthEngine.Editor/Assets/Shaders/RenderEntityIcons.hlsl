
cbuffer ConstantBuffer_Billboards : register(b0)
{
    float3 gCenter;
    uint gTextureIndex;
    float3 gExtents;
    float _CBPad1;
};

#define REGISTER_CBPerFrame b1
#include "CBPerFrame.hlsli"

Texture2D MaterialTextures[500] : register(t0, space0);

#include "Samplers.hlsli"

struct VertexOut
{
    uint VertexID;
};

float4 vs(uint _VertexID : SV_VertexID) : POSITIONT 
{
    VertexOut _VSOut;
    _VSOut.VertexID = _VertexID;
	return _VSOut;
}


struct GSOut
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
};

[maxvertexcount(4)]
void gs(
    point VertexOut input[1],
    inout TriangleStream<GSOut> output
)
{
    float4 _CenterProjection = mul(float4(gCenter, 1.0f), viewProj);
    _CenterProjection.xyzw /= _CenterProjection.w;
    
    float4 _ExtentsProjection = mul(float4(gExtents, 1.0f), viewProj);
    _ExtentsProjection.xyzw /= _ExtentsProjection.w;
    
    
    GSOut _GSOut;
    _GSOut.Position = float4(_CenterProjection.x - _ExtentsProjection.x, _CenterProjection.y + _ExtentsProjection.y, _CenterProjection.z, 1.0f);
    _GSOut.UV = float2(0.0f, 0.0f);
    output.Append(_GSOut);
    
    _GSOut.Position = float4(_CenterProjection.x + _ExtentsProjection.x, _CenterProjection.y + _ExtentsProjection.y, _CenterProjection.z, 1.0f);
    _GSOut.UV = float2(1.0f, 0.0f);
    output.Append(_GSOut);
    
    _GSOut.Position = float4(_CenterProjection.x - _ExtentsProjection.x, _CenterProjection.y - _ExtentsProjection.y, _CenterProjection.z, 1.0f);
    _GSOut.UV = float2(0.0f, 1.0f);
    output.Append(_GSOut);
    
    _GSOut.Position = float4(_CenterProjection.x + _ExtentsProjection.x, _CenterProjection.y - _ExtentsProjection.y, _CenterProjection.z, 1.0f);
    _GSOut.UV = float2(1.0f, 1.0f);
    output.Append(_GSOut);
}

float4 ps(GSOut _PixelIn) : SV_Target
{
    return MaterialTextures[gTextureIndex].Sample(sampler_linear_clamp, _PixelIn.UV);
}
