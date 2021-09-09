#include "MathHelper.hlsli"

/////////////////////////////////////////////////////////////////
//////////////// Constant Buffers
/////////////////////////////////////////////////////////////////

#define REGISTER_CBPerFrame b0
#include "CBPerFrame.hlsli"

#define REGISTER_CBUnfrequent b1
#include "CBUnfrequent.hlsli"

cbuffer ConstantBuffer_ScreenSpaceReflection : register(b2)
{
    float gMaxDistance; // The maximum distance for marching ray (in view space coordinates) 
    float gResolution; // The factor for specifying the fraction of pixels along the ray we'll go through in the first step 
    float gThickness; // The acceptable distance between screen sample and point on the ray (to be taken into account as a ray hit point)
    float gSteps;
}

///////////////////////////////////////////////////
//////////////// Textures
///////////////////////////////////////////////////

Texture2D<float4> tSpecular : register(t0);
Texture2D<float3> tNormal : register(t1);
Texture2D<float> tDepth : register(t2);
Texture2D<float4> tHDR : register(t3);


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
    float2 UV : TEXCOORD1;
    float2 PosCS : TEXCOORD2;
};

VertexOut vs(uint vertexID : SV_VertexID)
{
    VertexOut _VOut;
    _VOut.PosH = float4(VertexPositions[vertexID], 0.0f, 1.0f);
    _VOut.UV = VertexPositions[vertexID] * float2(0.5f, -0.5f) + (0.5f, 0.5f);
    _VOut.PosCS = _VOut.PosH.xy;
    
    return _VOut;
}

float4 ps(VertexOut _PixelIn) : SV_Taregt
{
    float3 _SpecularValues = tSpecular.Sample(sampler_point_wrap, _PixelIn.UV).xyz;
    
    // SpecularValues.y is "Metalness" and we determine reflective surface according to their metalness factor
    // if their metalness is less than refernce numebr (0.8f here) we don't move forward anymore.(clipping pixel)
    clip(_SpecularValues.y - 0.8f);
    
    float3 _NormalW = tNormal.Sample(sampler_point_wrap, _PixelIn.UV).xyz * 2.0f - 1.0f;
    float3 _NormalV = normalize(mul(_NormalW, (float3x3) View));
    
    float _DepthP = tDepth.Sample(sampler_point_wrap, _PixelIn.UV).x;
    float _DepthV = ConvertToLinearDepth(_DepthP, ProjectionValues.z, ProjectionValues.w);
    float4 _PosV = ReconstructViewPosition(_PixelIn.PosCS, _DepthV, ProjectionValues);
    float3 _VectorPosV = normalize(_PosV.xyz);
    
    float3 _ReflectV = reflect(_VectorPosV, _NormalV);
    
    float4 _StartV = _PosV;
    float4 _EndV = float4(_StartV.xyz + (_ReflectV * gMaxDistance), 1.0f);
    
    float4 _StartPixel = mul(_StartV, Projection);
    _StartPixel.xyz /= _StartPixel.w;
    _StartPixel.xy *= 0.5f;
    _StartPixel.xy += 0.5f;
    _StartPixel.xy *= gSceneViewportSize;
    
    float4 _EndPixel = mul(_EndV, Projection);
    _EndPixel.xyz /= _EndPixel.w;
    _EndPixel *= 0.5f;
    _EndPixel += 0.5f;
    _EndPixel *= gSceneViewportSize;
    
    float _DeltaX = _EndPixel.x - _StartPixel.x;
    float _DeltaY = _EndPixel.y - _StartPixel.y;
    
    float _UseX = abs(_DeltaX) >= abs(_DeltaY) ? 1.0f : 0.0f;
    float _Delta = lerp(abs(_DeltaX), abs(_DeltaY), _UseX) * clamp(gResolution, 0.0f, 1.0f);
    float2 _Increment = float2(_DeltaX, _DeltaY) / max(_Delta, 0.001f);
    
    float _Search0 = 0;
    float _Search1 = 0;
    
    uint _Hit0 = 0;
    uint _Hit1 = 0;
    
    float _ViewDistance = _StartV.z;
    float _Depth = gThickness;
    float _SampleDepthV = 0.0f;
    
    float2 _Pixel = _StartPixel.xy;
    float4 _UV = float4(_PixelIn.UV, 0.0f, 0.0f);
    
    float i = 0;
    
    for (i = 0; i < _Delta; ++i)
    {
        _Pixel.xy += _Increment;
        _UV.xy = _Pixel.xy / gSceneViewportSize;
        
        float _SampleDepthP = tDepth.Sample(sampler_point_wrap, _UV.xy).x;
        _SampleDepthV = ConvertToLinearDepth(_SampleDepthP, ProjectionValues.z, ProjectionValues.w);
        
        _Search1 = lerp
        (
            (_Pixel.y - _StartPixel.y) / _DeltaY,
            (_Pixel.x - _StartPixel.x) / _DeltaX,
            _UseX
        );
        
        _Search1 = saturate(_Search1);
        
        _ViewDistance = (_StartV.z * _EndV.z) / lerp(_EndV.z, _StartV.z, _Search1);
        _Depth = _ViewDistance - _SampleDepthV;
        
        if (_Depth > 0.0f && _Depth < gThickness)
        {
            _Hit0 = 1;
            break;
        }
        else
        {
            _Search0 = _Search1;
        }
    }
    
    _Search1 = _Search0 + ((_Search1 - _Search0) / 2.0f);
    
    float _Steps = gSteps * _Hit0;
    
    for (i = 0.0f; i < _Steps; ++i)
    {
        _Pixel = lerp(_StartPixel.xy, _EndPixel.xy, _Search1);
        _UV.xy = _Pixel / gSceneViewportSize;
        float _SampleDepthP = tDepth.Sample(sampler_point_wrap, _UV.xy);
        _SampleDepthV = ConvertToLinearDepth(_SampleDepthP, ProjectionValues.z, ProjectionValues.w);
        
        _ViewDistance = _StartV.z * _EndV.z / lerp(_EndV.z, _StartV.z, _Search1);
        _Depth = _ViewDistance - _SampleDepthV;
        
        if (_Depth < 0 && _Depth < gThickness)
        {
            _Hit1 = 1;
            _Search1 = _Search0 + ((_Search1 - _Search0) / 2.0f);
        }
        else
        {
            float _Temp = _Search1;
            float _Search1 = _Search1 + ((_Search1 - _Search0) / 2.0f);
            _Search0 = _Temp;
        }
    }
    
    float4 _HitPosV = ReconstructViewPosition(_UV.xy * 2.0f - 1.0f, _SampleDepthV, ProjectionValues);
    
    float _Visibility = _Hit1
    * (1.0f - max(dot(_ReflectV, _NormalV), 0.0f))
    * (1.0f - saturate(_Depth / gThickness))
    * (1.0f - saturate(distance(_HitPosV.xyz, _PosV.xyz) / gMaxDistance))
    * (_UV.x > 0.0f && _UV.x < 1.0f ? 1.0f : 0.0f)
    * (_UV.y > 0.0f && _UV.y <1.0f ? 1.0f : 0.0f);

    _Visibility = saturate(_Visibility);
    
    return float4(tHDR.Sample(sampler_point_borderBlack, _UV.xy).xyz, _Visibility);
}