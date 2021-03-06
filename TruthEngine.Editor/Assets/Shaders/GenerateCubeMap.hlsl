///////////////////////////////////////////////////
//////////////// Textures
///////////////////////////////////////////////////
Texture2D tMap : register(t0, space0);

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


static const float2 invAtan = float2(0.1591, 0.3183);

float2 SampleSphericalMap(float3 _PositionWorld)
{
    float2 uv = float2(atan2(_PositionWorld.x, _PositionWorld.z), acos(_PositionWorld.y));
    uv *= invAtan;
    uv.x += 0.5;
    return uv;
}

float4 ps(GSOutput _GsOut) : SV_Target
{
    float2 uv = SampleSphericalMap(normalize(_GsOut._PositionW)); // make sure to normalize localPos
    float3 color = tMap.Sample(sampler_linear, uv).rgb;
    
    //color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
    
    return float4(color, 1.0);
}
