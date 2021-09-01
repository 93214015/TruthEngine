
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
    uint VertexID : TEXCOORD;
};

VertexOut vs(uint _VertexID : SV_VertexID)
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
    float4 _CenterView = mul(float4(gCenter, 1.0f), View);
    
    
    GSOut _GSOut;
    _GSOut.Position = float4(_CenterView.x - gExtents.x, _CenterView.y + gExtents.y, _CenterView.z, 1.0f);
    _GSOut.Position = mul(_GSOut.Position, Projection);
    _GSOut.Position.xyzw /= _GSOut.Position.w;
    _GSOut.UV = float2(0.0f, 0.0f);
    output.Append(_GSOut);
    
    _GSOut.Position = float4(_CenterView.x + gExtents.x, _CenterView.y + gExtents.y, _CenterView.z, 1.0f);
    _GSOut.Position = mul(_GSOut.Position, Projection);
    _GSOut.Position.xyzw /= _GSOut.Position.w;
    _GSOut.UV = float2(1.0f, 0.0f);
    output.Append(_GSOut);
    
    _GSOut.Position = float4(_CenterView.x - gExtents.x, _CenterView.y - gExtents.y, _CenterView.z, 1.0f);
    _GSOut.Position = mul(_GSOut.Position, Projection);
    _GSOut.Position.xyzw /= _GSOut.Position.w;
    _GSOut.UV = float2(0.0f, 1.0f);
    output.Append(_GSOut);
    
    _GSOut.Position = float4(_CenterView.x + gExtents.x, _CenterView.y - gExtents.y, _CenterView.z, 1.0f);
    _GSOut.Position = mul(_GSOut.Position, Projection);
    _GSOut.Position.xyzw /= _GSOut.Position.w;
    _GSOut.UV = float2(1.0f, 1.0f);
    output.Append(_GSOut);
}

float4 ps(GSOut _PixelIn) : SV_Target
{
    return MaterialTextures[gTextureIndex].Sample(sampler_linear_clamp, _PixelIn.UV);
}
