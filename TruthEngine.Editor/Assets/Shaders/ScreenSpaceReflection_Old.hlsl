#include "MathHelper.hlsli"

/////////////////////////////////////////////////////////////////
//////////////// Constant Buffers
/////////////////////////////////////////////////////////////////


#define REGISTER_CBPerFrame b0
#include "CBPerFrame.hlsli"

cbuffer ConstantBuffer_SSReflection : register(b1)
{
    float gViewAngleThreshold : packoffset(c0.x);
    float gEdgeDistThreshold : packoffset(c0.y);
    float gDepthBias : packoffset(c0.z);
    float gReflectionScale : packoffset(c0.w);
};

///////////////////////////////////////////////////
//////////////// Textures
///////////////////////////////////////////////////

Texture2D tSpecular : register(t0);
Texture2D tNormal : register(t1);
Texture2D tDepth : register(t2);
Texture2D tHDR : register(t3);


/////////////////////////////////////////////////////////////////
//////////////// Samplers
/////////////////////////////////////////////////////////////////
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
    float2 UV : TEXCOORD1;
    float2 PosCS : TEXCOORD2;
};

VertexOut vs(uint vertexID : SV_VertexID)
{
    VertexOut _VOut;
    _VOut.PosH = float4(VertexPositions[vertexID], 0.0f, 1.0f);
    _VOut.UV = VertexPositions[vertexID] * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    _VOut.PosCS = _VOut.PosH.xy;
    
    return _VOut;
}

///////////////////////////////////////////////////
//////////////// Pixel Shader
///////////////////////////////////////////////////

// Pixel size in clip-space
// This is resulotion dependent
// Pick the minimum of the HDR width and height
static const float gPixelSize = 2.0 / 768.0f;

// Number of sampling steps
// This is resulotion dependent
// Pick the maximum of the HDR width and height
static const int gNumSteps = 1024;


float4 ps(VertexOut _PixelIn) : SV_Target
{
    float _Metallness = tSpecular.Sample(sampler_point_wrap, _PixelIn.UV).y;
    
    clip(_Metallness - 0.8f);
    
    float _DepthProj = tDepth.Sample(sampler_point_wrap, _PixelIn.UV).x;
    float _DepthView = ConvertToLinearDepth(_DepthProj, ProjectionValues.z, ProjectionValues.w);
    float4 _PosV = ReconstructViewPosition(_PixelIn.PosCS, _DepthView, ProjectionValues);
    
    float3 _NormalW = tNormal.Sample(sampler_point_wrap, _PixelIn.UV).xyz;
    _NormalW = _NormalW * 2.0f - 1.0f;
    float3 _NormalV = normalize(mul(_NormalW, (float3x3)View));
    
    float3 _EyeToPixel = normalize(_PosV.xyz);
    
    float3 _ReflectV = reflect(_EyeToPixel, _NormalV);
    
    float4 _ReflectColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    if(_ReflectV.z >= gViewAngleThreshold)
    {
        
        float _ViewAngleThresholdInv = 1.0f - gViewAngleThreshold;
        float _ViewAngleFade = saturate(3.0f * (_ReflectV.z - gViewAngleThreshold) / _ViewAngleThresholdInv);
        
        float3 _ReflectPosV = _PosV.xyz + _ReflectV;
        float3 _ReflectPosCS = mul(float4(_ReflectPosV, 1.0f), Projection).xyz / _ReflectPosV.z;
        float3 _ReflectCS = _ReflectPosCS - float3(_PixelIn.PosCS, _DepthProj);
        
        float _ReflectScale = gPixelSize / length(_ReflectCS.xy);
        _ReflectCS *= _ReflectScale;
        
        float2 _UVStepSize = _ReflectCS.xy * float2(0.5f, -0.5f);

        float4 _RayPlane;
        float3 _VRight = cross(_EyeToPixel, _ReflectV);
        _RayPlane.xyz = normalize(cross(_ReflectV, _VRight));
        _RayPlane.w = dot(_RayPlane.xyz, _PosV.xyz);
        
        float2 _SampleUV = _PixelIn.PosCS.xy + _ReflectCS.xy;
        _SampleUV = _SampleUV * float2(0.5f, -0.5f) + 0.5f;
        
        for (uint _CurrStep = 0; _CurrStep < gNumSteps; ++_CurrStep)
        {
            float _SampleDepthProj = tDepth.Sample(sampler_point_wrap, _SampleUV).x;

            if(_SampleDepthProj > 0.9999f)
            {
                continue;
            }

            float _SampleDepthView = ConvertToLinearDepth(_SampleDepthProj, ProjectionValues.z, ProjectionValues.w);
            
            float3 _SamplePosView = ReconstructViewPosition(_PixelIn.PosCS + _ReflectCS.xy * (_CurrStep + 1.0f), _SampleDepthView, ProjectionValues).xyz;

            if(_RayPlane.w >= dot(_RayPlane.xyz, _SamplePosView) + gDepthBias)
            {
                float3 _FinalPosView = _PosV.xyz + (_ReflectV / abs(_ReflectV.z)) * abs(_SampleDepthView - _PosV.z + gDepthBias);
                float2 _FinalPosCS = _FinalPosView.xy / ProjectionValues.xy / _FinalPosView.z;
                _SampleUV = _FinalPosCS.xy * float2(0.5f, -0.5f) + 0.5f;
                
                _ReflectColor.xyz = tHDR.SampleLevel(sampler_point_wrap, _SampleUV, 0.0f).xyz;
                
                float _EdgeFade = saturate(distance(_SampleUV, float2(0.5f, 0.5f)) * 2.0 - gEdgeDistThreshold);
                
                _ReflectColor.w = min(_ViewAngleFade, 1.0f - _EdgeFade * _EdgeFade);
                _ReflectColor.w *= gReflectionScale;
                
                _CurrStep = gNumSteps;
            }
            
            _SampleUV += _UVStepSize;

        }

    }
    
    return _ReflectColor;
    
}

float4 ps_Book(VertexOut _PixelIn) : SV_Target
{
    float _Metallness = tSpecular.Sample(sampler_point_wrap, _PixelIn.UV).y;
    
    clip(_Metallness - 0.8f);
    
    float _DepthProj = tDepth.Sample(sampler_point_wrap, _PixelIn.UV).x;
    float _DepthView = ConvertToLinearDepth(_DepthProj, ProjectionValues.z, ProjectionValues.w);
    float4 _PosV = ReconstructViewPosition(_PixelIn.PosCS, _DepthView, ProjectionValues);
    float3 _PosCS = float3(_PixelIn.PosCS, _DepthProj);
    
    float3 _NormalW = tNormal.Sample(sampler_point_wrap, _PixelIn.UV).xyz;
    _NormalW = _NormalW * 2.0f - 1.0f;
    float3 _NormalV = normalize(mul(_NormalW, (float3x3) View));
    
    float3 _EyeToPixel = normalize(_PosV.xyz);
    
    float3 _ReflectV = reflect(_EyeToPixel, _NormalV);
    
    float4 _ReflectColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    if (_ReflectV.z > gViewAngleThreshold)
    {
        
        float _ViewAngleThresholdInv = 1.0f - gViewAngleThreshold;
        float _ViewAngleFade = (_ReflectV.z - gViewAngleThreshold) / _ViewAngleThresholdInv;
        
        float3 _ReflectPosV = _PosV.xyz + _ReflectV;
        float3 _ReflectPosCS = mul(float4(_ReflectPosV, 1.0f), Projection).xyz / _ReflectPosV.z;
        float3 _ReflectCS = _ReflectPosCS - _PosCS;
        
        float _ReflectScale = gPixelSize / length(_ReflectCS.xy);
        _ReflectCS *= _ReflectScale;
        
        float3 _SampleOffset = _PosCS + _ReflectCS;
        _SampleOffset.xy = _SampleOffset.xy * float2(0.5f, -0.5f) + 0.5f;
        
        float3 _LastOffset = _PosCS;
        _LastOffset.xy = _LastOffset.xy * float2(0.5, -5.0f) + 0.5f;
        
        _ReflectCS *= float3(0.5, -0.5f, 1.0f);
        
        for (uint _CurrStep = 0; _CurrStep < gNumSteps; ++_CurrStep)
        {
            float _SampleDepthProj = tDepth.Sample(sampler_point_wrap, _SampleOffset.xy).x + gDepthBias;
            
            if (_SampleDepthProj < _SampleOffset.z)
            {
                _SampleOffset.xy = _LastOffset.xy + _ReflectCS.xy * (_SampleOffset.z - _SampleDepthProj);

                _ReflectColor.xyz = tHDR.Sample(sampler_point_wrap, _SampleOffset.xy).xyz;

                float _EdgeFade = saturate(distance(_SampleOffset.xy, float2(0.5f, 0.5f)) * 2.0 - gEdgeDistThreshold);

                _ReflectColor.w = min(_ViewAngleFade, 1.0f - _EdgeFade * _EdgeFade);
                _ReflectColor.w *= gReflectionScale;


                _CurrStep = gNumSteps;
            }
            
            _LastOffset = _SampleOffset;
            _SampleOffset += _ReflectCS;            

        }

    }
    
    return _ReflectColor;
    
}