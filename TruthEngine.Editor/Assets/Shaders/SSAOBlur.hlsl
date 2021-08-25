/////////////////////////////////////////////////////////////////
//////////////// Textures
/////////////////////////////////////////////////////////////////
Texture2D tInput : register(t0);

/////////////////////////////////////////////////////////////////
//////////////// Samplers
/////////////////////////////////////////////////////////////////
#include "Samplers.hlsli"


/////////////////////////////////////////////////////////////////
//////////////// Vertex Shader
/////////////////////////////////////////////////////////////////

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
    float2 UV : TEXCOORD0;
};

VertexOut vs(uint vertexID : SV_VertexID)
{
    VertexOut _VOut;
    _VOut.PosH = float4(VertexPositions[vertexID], 0.0f, 1.0f);
    _VOut.UV = VertexPositions[vertexID] * float2(0.5f, -0.5f) + (0.5f, 0.5f);
    
    return _VOut;
}


/////////////////////////////////////////////////////////////////
//////////////// Pixel Shader
/////////////////////////////////////////////////////////////////


float ps(VertexOut _PixelIn) : SV_Target
{
    
    float _Result = 0.0f;
    
    for (int i = -2; i <= 2; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            _Result += tInput.Sample(sampler_linear_clamp, _PixelIn.UV, int2(i, j)).r;
        }
    }

    _Result /= 25.0f;
    
    return _Result;
    
}