#include "Lights.hlsli"
#include "MathHelper.hlsli"


/////////////////////////////////////////////////////////////////
//////////////// Constant Buffers
/////////////////////////////////////////////////////////////////

#define REGISTER_CBPerFrame b0
#include "CBPerFrame.hlsli"

/////////////////////////////////////////////////////////////////
//////////////// Textures
/////////////////////////////////////////////////////////////////

Texture2D<float4> tColor : register(t0);
Texture2D<float3> tNormal : register(t1);
Texture2D<float> tDepth : register(t2);

/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//////////////// Samplers
/////////////////////////////////////////////////////////////////
#include "Samplers.hlsli"


static const float2 VertexPositions[4] =
{
    { -1.0f, 1.0f },
    { -1.0f, -1.0f },
    { 1.0f, -1.0f },
    { 1.0f, 1.0 }
};

struct VertexOut
{
    float4 PosH : SV_Position;
    float2 UV : TEXCOORD0;
    float2 PosCS : TEXCOORD1;
};

float4 vs(uint vertexID : SV_VertexID) : SV_POSITION
{
    VertexOut _VOut;
    _VOut.PosH = float4(VertexPositions[vertexID], 0.0f, 1.0f);
    _VOut.UV = float2((_VOut.PosH.x * 0.5) + 0.5, (_VOut.PosH.y * -0.5f) + 0.5f);
    _VOut.PosCS = _VOut.PosH.xy;
    
    return _VOut;
}

float4 ps(VertexOut _VOut) : SV_Target
{
    float _DepthProj = tDepth.Sample(sampler_point_wrap, _VOut.UV);
    
    float _DepthView = ConvertToLinearDepth(_DepthProj, ProjectionValues.z, ProjectionValues.w);

}