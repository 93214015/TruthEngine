#include "MathHelper.hlsli"

/////////////////////////////////////////////////////////////////
//////////////// Textures
/////////////////////////////////////////////////////////////////
Texture2D<float> tDepth;
Texture2D<float3> tNormal;
Texture2D<float3> tNoise;

/////////////////////////////////////////////////////////////////
//////////////// Constant Buffers
/////////////////////////////////////////////////////////////////

#define REGISTER_CBPerFrame b0
#include "CBPerFrame.hlsli"

#define REGISTER_CBUnfrequent b1
#include "CBUnfrequent.hlsli"

cbuffer CBSSAO : register(b2)
{
    float3 KernelSamples[64];
};



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



/////////////////////////////////////////////////////////////////
//////////////// Pixel Shader
/////////////////////////////////////////////////////////////////

#define NOISE_TEXTURE_SIZE 4.0f // 4x4

float4 ps(VertexOut _PixelIn) : SV_Target
{
    
    float _DepthProjection = tDepth.Sample(sampler_point_borderWhite, _PixelIn.UV);
    
    float _DepthView = ConvertToLinearDepth(_DepthProjection, ProjectionValues.z, ProjectionValues.w);
    
    float4 _PosView = ReconstructViewPosition(_PixelIn.PosCS, _DepthView, ProjectionValues);
    
    float3 _NormalWorld = tNormal.Sample(sampler_point_borderWhite, _PixelIn.UV) * 2.0f - 1.0f;
    
    float3 _NormalView = mul(_NormalWorld, (float3x3)View);
    
    const float2 _NoiseScale = gSceneViewportSize / NOISE_TEXTURE_SIZE.xx;
    
    float3 _RandomVector = tNoise.Sample(sampler_point_wrap, _PixelIn.UV * _NoiseScale);
    
    float3 _Tanget = normalize(_RandomVector - _NormalView * dot(_NormalView, _RandomVector)); // Gramm-Schmidt process
    
    float3 _BiTanget = cross(_NormalView, _Tanget);
    
    float
    
}