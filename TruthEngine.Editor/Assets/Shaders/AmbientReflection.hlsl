#include "MathHelper.hlsli"
#include "LightsPBR.hlsli"

/////////////////////////////////////////////////////////////////
//////////////// Constant Buffers
/////////////////////////////////////////////////////////////////
#define REGISTER_CBPerFrame b0
#include "CBPerFrame.hlsli"

///////////////////////////////////////////////////
//////////////// Textures 
///////////////////////////////////////////////////

Texture2D tNormal : register(t0);
Texture2D tDepth : register(t1);
Texture2D tColor : register(t2);

Texture2D tReflectionUV : register(t3);
Texture2D tHDRScene : register(t4);
Texture2D tHDRSceneBlurred : register(t5);
Texture2D tSpecularity : register(t6);
TextureCube tIBLSpecular : register(t7);
Texture2D<float2> tPrecomputedBRDF : register(t8);


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
    
    
    float3 _NormalW = tNormal.Sample(sampler_point_wrap, _PixelIn.UV).xyz * 2.0f - 1.0f;
    float3 _NormalV = normalize(mul(_NormalW, (float3x3) View));
    
    
    float _DepthP = tDepth.Sample(sampler_point_wrap, _PixelIn.UV).x;
    float _DepthV = ConvertToLinearDepth(_DepthP, ProjectionValues.z, ProjectionValues.w);
    float4 _PosV = ReconstructViewPosition(_PixelIn.PosCS, _DepthV, ProjectionValues);
    float3 _PosVDir = normalize(_PosV.xyz);
    
    
    /////////////////////////////////////////
    ///////     Ambient Lighting
    /////////////////////////////////////////
    
    float _NdotV = max(dot(_NormalV, -_PosVDir), 0.0f);
    
    float4 _Color = tColor.Sample(sampler_point_wrap, _PixelIn.UV);
    float4 _Specularity = tSpecularity.Sample(sampler_point_wrap, _PixelIn.UV);

    float3 _F0 = float3(0.04f, 0.04f, 0.04f);
    _F0 = lerp(_F0, _Color.xyz, _Specularity.y);
    
    float3 _Ks = FresnelSchlickRoughness(_F0, _NdotV, _Specularity.x);
    
    float4 _SceneReflectionFactors = tReflectionUV.Sample(sampler_point_wrap, _PixelIn.UV);
    
    float4 _AmbientReflection = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    [branch]
    if (_SceneReflectionFactors.z >= 0.1f)
    {
        float3 _SceneColor = tHDRScene.Sample(sampler_linear, _SceneReflectionFactors.xy).xyz;
        float3 _SceneColorBlurred = tHDRSceneBlurred.Sample(sampler_linear, _SceneReflectionFactors.xy).xyz;
        
        _AmbientReflection = float4(lerp(_SceneColor, _SceneColorBlurred, _Specularity.x) * _Ks, _SceneReflectionFactors.z);
    }
    else
    {
        float3 _ReflectVectorV = reflect(_PosVDir, _NormalV);
        float3 _ReflectVectorW = normalize(mul(_ReflectVectorV, (float3x3) ViewInverse));
        const float MAX_REFLECTION_LOD = 4.0f;
        float3 _PrefilteredIBLSpecular = tIBLSpecular.SampleLevel(sampler_linear, _ReflectVectorW, _Specularity.x * MAX_REFLECTION_LOD).rgb;
        float2 _PrecomputeBRDF = tPrecomputedBRDF.Sample(sampler_linear, float2(_NdotV, _Specularity.x)).xy;
        _AmbientReflection = float4(_PrefilteredIBLSpecular * (_Ks * _PrecomputeBRDF.x + _PrecomputeBRDF.y), 1.0f);

        //float _SSAO = tSSAO.Sample(sampler_linear_clamp, _VOut.UV).x;
        //_AmbientReflection.xyz *= _SSAO;
    }

    _AmbientReflection.xyz *= _Specularity.z; // Material SSAO
    
    return _AmbientReflection;
}