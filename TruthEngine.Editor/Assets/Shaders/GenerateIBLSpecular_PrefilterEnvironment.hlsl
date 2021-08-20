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

#define SAMPLE_COUNT 1024u

cbuffer CBData : register(b0)
{
    float gRoughness;
    float3 _CBDataPad0;
};


float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    float a = roughness * roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent = normalize(cross(up, N));
    float3 bitangent = cross(N, tangent);
	
    float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}


float4 ps(GSOutput _GsOut) : SV_Target
{
    float3 _Normal = normalize(_GsOut._PositionW);
    float3 _Reflect = _Normal;
    float3 _View = _Reflect;
    
    float _TotalWeight = 0.0f;
    float3 _PrefilteredColor = float3(0.0f, 0.0f, 0.0f);
    
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        float2 _Xi = Hammersley(i, SAMPLE_COUNT);
        float3 _H = ImportanceSampleGGX(_Xi, _Normal, gRoughness);
        float3 _L = normalize(2.0f * dot(_View, _H) * _H - _View);
        
        float _NdotL = max(dot(_Normal, _L), 0.0f);
        if(_NdotL > 0.0f)
        {
            _PrefilteredColor += tMap.Sample(sampler_linear, _L).rgb * _NdotL.xxx;
            _TotalWeight += _NdotL;
        }

    }
    
    _PrefilteredColor /= _TotalWeight;
    
    return float4(_PrefilteredColor, 1.0f);

}
