#include "MathHelper.hlsli"

///////////////////////////////////////////////////
//////////////// Textures
///////////////////////////////////////////////////
TextureCube tMap : register(t0, space0);

///////////////////////////////////////////////////
//////////////// Sampler
///////////////////////////////////////////////////
#include "Samplers.hlsli"


///////////////////////////////////////////////////
//////////////// Const Values
///////////////////////////////////////////////////

const static float4 _PositionClipSpace[4] =
{
    float4(-1.0f, 1.0f, 1.0f, 1.0f),
    float4(1.0f, 1.0f, 1.0f, 1.0f),
    float4(-1.0f, -1.0f, 1.0f, 1.0f),
    float4(1.0f, -1.0f, 1.0f, 1.0f)
};

const static float4 _PositionsWorld[6][4] =
{
    // Right Face
    {
        float4(1.0f, 1.0f, 1.0f, 1.0f),
        float4(1.0f, 1.0f, -1.0f, 1.0f),
        float4(1.0f, -1.0f, 1.0f, 1.0f),
        float4(1.0f, -1.0f, -1.0f, 1.0f),
    },
    // Left Face
    {
        float4(-1.0f, 1.0f, -1.0f, 1.0f),
        float4(-1.0f, 1.0f, 1.0f, 1.0f),
        float4(-1.0f, -1.0f, -1.0f, 1.0f),
        float4(-1.0f, -1.0f, 1.0f, 1.0f),
    },
    // Top Face
    {
        float4(-1.0f, 1.0f, -1.0f, 1.0f),
        float4(1.0f, 1.0f, -1.0f, 1.0f),
        float4(-1.0f, 1.0f, 1.0f, 1.0f),
        float4(1.0f, 1.0f, 1.0f, 1.0f),
    },
    // Bottom Face
    {
        float4(-1.0f, -1.0f, 1.0f, 1.0f),
        float4(1.0f, -1.0f, 1.0f, 1.0f),
        float4(-1.0f, -1.0f, -1.0f, 1.0f),
        float4(1.0f, -1.0f, -1.0f, 1.0f),
    },
    //Front Face
    {
        float4(-1.0f, 1.0f, 1.0f, 1.0f),
        float4(1.0f, 1.0f, 1.0f, 1.0f),
        float4(-1.0f, -1.0f, 1.0f, 1.0f),
        float4(1.0f, -1.0f, 1.0f, 1.0f),
    },
    // Back Face
    {
        float4(1.0f, 1.0f, -1.0f, 1.0f),
        float4(-1.0f, 1.0f, -1.0f, 1.0f),
        float4(1.0f, -1.0f, -1.0f, 1.0f),
        float4(-1.0f, -1.0f, -1.0f, 1.0f),
    },
     
    
};

///////////////////////////////////////////////////
//////////////// Vertex Shader
///////////////////////////////////////////////////

struct VertexOut
{
    uint _FaceID : TEXCOORD0;
};

VertexOut vs(uint _VertexID : SV_VertexID)
{
    VertexOut _VOut;
    _VOut._FaceID = _VertexID;
    return _VOut;
	
}


///////////////////////////////////////////////////
//////////////// Geometry Shader
///////////////////////////////////////////////////

struct GSOutput
{
    float4 _Position : SV_POSITION;
    float3 _PositionW : TEXCOORD0;
    uint _RTVIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(24)]
void gs(
	point VertexOut input[1],
	inout TriangleStream<GSOutput> output
)
{
    
    for (uint _RTVIndex = 0; _RTVIndex < 6; _RTVIndex++)
    {
        GSOutput element;
        element._RTVIndex = _RTVIndex;

        for (uint i = 0; i < 4; i++)
        {
            element._Position = _PositionClipSpace[i];
            element._PositionW = normalize(_PositionsWorld[_RTVIndex][i].xyz);
            output.Append(element);
        }
        
        output.RestartStrip();
    }
    
}


///////////////////////////////////////////////////
//////////////// Pixel Shader
///////////////////////////////////////////////////

#define SAMPLE_DELTA 0.025f


float4 ps(GSOutput _GsOut) : SV_Target
{
    float3 _Normal = normalize(_GsOut._PositionW);
    float3 _Up = float3(0.0f, 1.0f, 0.0f);
    float3 _Right = normalize(cross(_Normal, _Up));
    _Up = normalize(cross(_Normal, _Right));
    
    float _SampleCount = 0.0f;
    
    float3 _Irrediance = float3(0.0f, 0.0f, 0.0f);
    
    for (float _Theta = 0.0f; _Theta < PI * 2.0f; _Theta += SAMPLE_DELTA)
    {
        for (float _Phi = 0.0f; _Phi < PI * 0.5f; _Phi += SAMPLE_DELTA)
        {
            //Spherical to cartesian
            float3 _TangentSample = float3(sin(_Phi) * cos(_Theta), sin(_Phi) * sin(_Theta), cos(_Phi));
            //Tangent space to world space
            float3 _SampleVec = _TangentSample.x * _Right + _TangentSample.y * _Up + _TangentSample.z * _Normal;
            _SampleVec = normalize(_SampleVec);
            
            _Irrediance += tMap.Sample(sampler_linear, _SampleVec).xyz * sin(_Phi) * cos(_Phi);
            _SampleCount++;
        }

    }
    _Irrediance = _Irrediance * PI / _SampleCount;
    
    
        
    //color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
    return float4(_Irrediance, 1.0);
}
