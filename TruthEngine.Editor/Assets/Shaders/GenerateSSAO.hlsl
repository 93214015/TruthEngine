#include "MathHelper.hlsli"

/////////////////////////////////////////////////////////////////
//////////////// Textures
/////////////////////////////////////////////////////////////////
Texture2D<float> tDepth : register(t0);
Texture2D<float3> tNormal : register(t1);
Texture2D<float3> tNoise : register(t2);

/////////////////////////////////////////////////////////////////
//////////////// Constant Buffers
/////////////////////////////////////////////////////////////////

#define REGISTER_CBPerFrame b0
#include "CBPerFrame.hlsli"

#define REGISTER_CBUnfrequent b1
#include "CBUnfrequent.hlsli"

cbuffer CBSSAO : register(b2)
{
    float gSSAORadius;
    float gSSAODepthBias;
    float2 _CBSSAOPad0;

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
    _VOut.UV = VertexPositions[vertexID] * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    _VOut.PosCS = _VOut.PosH.xy;
    
    return _VOut;
}



/////////////////////////////////////////////////////////////////
//////////////// Pixel Shader
/////////////////////////////////////////////////////////////////

#define NOISE_TEXTURE_SIZE 4.0f // 4x4
#define NOISE_TEXTURE_SIZE_INV (1.0f/NOISE_TEXTURE_SIZE) // 4x4
#define KernelSize 64u
#define KernelSize_Inv (1.0f / KernelSize)
#define Radius 0.5f
#define DepthBias 0.025f

float ps(VertexOut _PixelIn) : SV_Target
{
    
    const float _DepthProjection = tDepth.Sample(sampler_point_borderWhite, _PixelIn.UV);
    
    const float _DepthView = ConvertToLinearDepth(_DepthProjection, ProjectionValues.z, ProjectionValues.w);
    
    const float4 _PosView = ReconstructViewPosition(_PixelIn.PosCS, _DepthView, ProjectionValues);
    
    const float3 _NormalWorld = tNormal.Sample(sampler_point_borderWhite, _PixelIn.UV) * 2.0f - 1.0f;
    
    const float3 _NormalView = normalize(mul(_NormalWorld, (float3x3) View));
    
    const float2 _NoiseScale = gSceneViewportSize * NOISE_TEXTURE_SIZE_INV;
    
    float3 _RandomVector = tNoise.Sample(sampler_point_wrap, _PixelIn.UV * _NoiseScale);
    
    float3 _Tangent = normalize(_RandomVector - _NormalView *  dot(_NormalView, _RandomVector)); // Gramm-Schmidt process
    
    float3 _BiTangent = cross(_NormalView, _Tangent);
    
    float3x3 _TBN = float3x3(_Tangent, _BiTangent, _NormalView);

    float _Occlusion = 0.0f;
    
    for (uint i = 0; i < KernelSize; ++i)
    {
        float3 _OffsetPos = mul(KernelSamples[i], _TBN);
        _OffsetPos = _PosView.xyz + (_OffsetPos * gSSAORadius);
        float4 _Offset = float4(_OffsetPos, 1.0f);
        
        _Offset = mul(_Offset, Projection);
        _Offset.xyz /= _Offset.w;
        float2 _SampleUV = (_Offset.xy * float2(0.5f, -0.5f)) + float2(0.5f, 0.5f);
        
        float _SampleDepthProjection = tDepth.Sample(sampler_point_borderWhite, _SampleUV);
        float _SampleDepthView = ConvertToLinearDepth(_SampleDepthProjection, ProjectionValues.z, ProjectionValues.w);

        float _RangeCheck = smoothstep(0.0f, 1.0f, gSSAORadius / abs(_SampleDepthView - _DepthView));
        
        _Occlusion += ((_SampleDepthView + gSSAODepthBias < _OffsetPos.z) ? 1.0 : 0.0f) * _RangeCheck;

    }
    
    _Occlusion = 1.0f - (_Occlusion * KernelSize_Inv);
    
    return _Occlusion;
    
}