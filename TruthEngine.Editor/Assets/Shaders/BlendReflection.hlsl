///////////////////////////////////////////////////
//////////////// Textures 
///////////////////////////////////////////////////
Texture2D tReflection : register(t0);

///////////////////////////////////////////////////
//////////////// Samplers
///////////////////////////////////////////////////
#include "Samplers.hlsli"


///////////////////////////////////////////////////
//////////////// Vertex Shader
///////////////////////////////////////////////////

//Defined For TriangleStrip Topology
static const float2 VertexPositions[4] =
{
    float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

struct VertexOut
{
    float4 PosH : SV_Position;
};

VertexOut vs(uint vertexID : SV_VertexID)
{
    VertexOut _VOut;
    _VOut.PosH = float4(VertexPositions[vertexID], 0.0f, 1.0f);
    return _VOut;
}


///////////////////////////////////////////////////
//////////////// Pixel Shader
///////////////////////////////////////////////////

float4 ps(VertexOut _PixelIn) : SV_Target
{
    return tReflection.Load(int3(_PixelIn.PosH.xy, 0));
}