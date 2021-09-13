

cbuffer CBPerObject : register(b0)
{
    float3 gExtents;
    float __CBPerObject_Pad0;
    float3 gCenter;
    float __CBPerObject_Pad1;
};

#define REGISTER_CBPerFrame b1
#include "CBPerFrame.hlsli"

//struct VertexIn
//{
//    float3 PosL : POSITION;
//};

//const static float3 Faces[4][] =
//{
//    {
//        float3(1.0f, -1.0, 1.0f),
//        float3(1.0f, 1.0, 1.0f),
//        float3(1.0f, 1.0, -1.0f),
//        float3(1.0f, -1.0, -1.0f),
//        float3(1.0f, -1.0, 1.0f),
//        float3(-1.0f, -1.0, 1.0f),
//        float3(-1.0f, 1.0, 1.0f),
//        float3(1.0f, 1.0, 1.0f)
//    },
//    {
//        float3(-1.0f, 1.0, 1.0f),
//        float3(-1.0f, 1.0, -1.0f),
//        float3(-1.0f, -1.0, -1.0f),
//        float3(-1.0f, -1.0, 1.0f)
//    },
//    {
//        float3(-1.0f, 1.0, -1.0f),
//        float3(1.0f, 1.0, -1.0f)
//    },
//    {
//        float3(-1.0f, -1.0, -1.0f),
//        float3(1.0f, -1.0, -1.0f)
//    }
//};

const static float3 Points0[8] =
{
    float3(1.0f, -1.0, 1.0f),
    float3(1.0f, 1.0, 1.0f),
    float3(1.0f, 1.0, -1.0f),
    float3(1.0f, -1.0, -1.0f),
    float3(1.0f, -1.0, 1.0f),
    float3(-1.0f, -1.0, 1.0f),
    float3(-1.0f, 1.0, 1.0f),
    float3(1.0f, 1.0, 1.0f)
};

const static float3 Points1[4] =
{
    float3(-1.0f, 1.0, 1.0f),
    float3(-1.0f, 1.0, -1.0f),
    float3(-1.0f, -1.0, -1.0f),
    float3(-1.0f, -1.0, 1.0f)
};

const static float3 Points2[2] =
{
    float3(-1.0f, 1.0, -1.0f),
    float3(1.0f, 1.0, -1.0f)
};

const static float3 Points3[2] =
{
    float3(-1.0f, -1.0, -1.0f),
    float3(1.0f, -1.0, -1.0f)
};

struct VertexOut
{
    uint VertexID : TEXCOORD0;
};

VertexOut vs(uint _VertexID : SV_VertexID)
{
    VertexOut _VOut;
    _VOut.VertexID = _VertexID;
    return _VOut;
}


struct GSOut
{
    float4 Position : SV_Position;
};

[maxvertexcount(16)]
void gs(
    point VertexOut input[1],
    inout LineStream<GSOut> output
)
{
    
    GSOut _GSOut;
    
    [unroll]
    for (uint i = 0; i < 8; ++i)
    {
        float4 _pos = float4((Points0[i] * gExtents) + gCenter, 1.0f);
        _pos = mul(_pos, viewProj);
        _GSOut.Position = _pos;
        output.Append(_GSOut);
    }
    
    output.RestartStrip();
    
    [unroll]
    for (uint j = 0; j < 4; ++j)
    {
        float4 _pos = float4((Points1[j] * gExtents) + gCenter, 1.0f);
        _pos = mul(_pos, viewProj);
        _GSOut.Position = _pos;
        output.Append(_GSOut);
    }
    
    output.RestartStrip();
    
    [unroll]
    for (uint k = 0; k < 2; ++k)
    {
        float4 _pos = float4((Points2[k] * gExtents) + gCenter, 1.0f);
        _pos = mul(_pos, viewProj);
        _GSOut.Position = _pos;
        output.Append(_GSOut);
    }
    
    output.RestartStrip();
    
    [unroll]
    for (uint l = 0; l < 2; ++l)
    {
        float4 _pos = float4((Points3[l] * gExtents) + gCenter, 1.0f);
        _pos = mul(_pos, viewProj);
        _GSOut.Position = _pos;
        output.Append(_GSOut);
    }
    
}

float4 ps(GSOut _PixelIn) : SV_Target
{
    return float4(0.5f, 0.8f, 0.25f, 1.0f);
}