#include "MathHelper.hlsli"

/////////////////////////////////////////////////////////////////
//////////////// Constant Buffers
/////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////
//////////////// Textures 
///////////////////////////////////////////////////
Texture2D tReflectionUV : register(t0);
Texture2D tHDRScene : register(t1);
Texture2D tHDRSceneBlurred : register(t2);
Texture2D tSpecularity : register(t3);


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
    float2 UV : TEXCOORD0;
    float2 PosCS : TEXCOORD1;
};

VertexOut vs(uint vertexID : SV_VertexID)
{
    VertexOut _VOut;
    _VOut.PosH = float4(VertexPositions[vertexID], 0.0f, 1.0f);
    _VOut.UV = VertexPositions[vertexID] * float2(0.5f, -0.5f) + (0.5f, 0.5f);
    _VOut.PosCS = _VOut.PosH.xy;
    return _VOut;
}


///////////////////////////////////////////////////
//////////////// Pixel Shader
///////////////////////////////////////////////////

float4 ps(VertexOut _PixelIn) : SV_Target
{
    
    float3 _ReflectionColor = 0.0f.xxx;
    
    float4 _ReflectionUV = tReflectionUV.Sample(sampler_point_borderBlack, _PixelIn.UV);
    
    if(_ReflectionUV.z >= 0.1)
    {
        float4 _Specularity = tSpecularity.Sample(sampler_point_borderBlack, _PixelIn.UV);
        
        float3 _SceneColor = tHDRScene.Sample(sampler_linear, _ReflectionUV.xy).xyz;
        float3 _SceneColorBlurred = tHDRSceneBlurred.Sample(sampler_linear, _ReflectionUV.xy).xyz;
        
        _ReflectionColor = lerp(_SceneColor, _SceneColorBlurred, _Specularity.x);
    }
    
    return float4(_ReflectionColor, _ReflectionUV.z);
}