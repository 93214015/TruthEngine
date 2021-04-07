

static const float2 arrBasePos[4] =
{
    float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

static const float2 arrUV[4] =
{
    float2(0.0, 0.0),
	float2(1.0, 0.0),
	float2(0.0, 1.0),
	float2(1.0, 1.0),
};

//-----------------------------------------------------------------------------------------
// Vertex shader
//-----------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position : SV_Position; // vertex position 
    float2 UV : TEXCOORD0;
};

VS_OUTPUT FullScreenQuadVS(uint VertexID : SV_VertexID)
{
    VS_OUTPUT Output;

    Output.Position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);
    Output.UV = arrUV[VertexID].xy;
    
    return Output;
}

//-----------------------------------------------------------------------------------------
// Pixel shader
//-----------------------------------------------------------------------------------------

cbuffer FinalPassConstants : register(b0)
{
	// Tone mapping
    float MiddleGrey : packoffset(c0.x);
    float LumWhiteSqr : packoffset(c0.y);
    float BloomScale : packoffset(c0.z);
    float pad0 : packoffset(c0.w);
    
    float2 gProjectionValues : packoffset(c1.x);
    float2 gDOFFarValues : packoffset(c1.z);
}


Texture2D<float4> tHDRTex : register(t0);
Texture2D<float4> tHDRBlurred : register(t1);
Texture2D<float4> tBloomTex : register(t2);
Texture2D<float> tDepthTex : register(t3);
StructuredBuffer<float> AvgLum : register(t4);


sampler sampler_linear : register(s0);
sampler sampler_point_borderBlack : register(s1);
sampler sampler_point_borderWhite : register(s2);
SamplerComparisonState samplerComparison_great_point_borderBlack : register(s3);
sampler sampler_point_wrap : register(s4);
SamplerComparisonState samplerComparison_less_point_borderWhite : register(s5);

static const float3 LUM_FACTOR = float3(0.299, 0.587, 0.114);

float ConvertZToLinearDepth(float _ClipDepth)
{
    return gProjectionValues.x / (_ClipDepth + gProjectionValues.y);
}

float3 DistanceDOF(float3 _ColorFocus, float3 _ColorBlurred, float _Depth)
{
    //Find the depth based blur factor
    float _BlurFactor = saturate((_Depth - gDOFFarValues.x) * gDOFFarValues.y);
    
    return lerp(_ColorFocus, _ColorBlurred, _BlurFactor);
}

float3 ToneMapping(float3 HDRColor)
{
	// Find the luminance scale for the current pixel
    float LScale = dot(HDRColor, LUM_FACTOR);
    float _AverageLuminance = AvgLum[0];
    LScale *= MiddleGrey / _AverageLuminance;
    LScale = (LScale + LScale * LScale / LumWhiteSqr) / (1.0 + LScale);
	
	// Apply the luminance scale to the pixels color
    return HDRColor * LScale;
}

float4 FinalPassPS(VS_OUTPUT In) : SV_TARGET
{
	// Get the color sample
    float3 _Color = tHDRTex.Sample(sampler_point_borderWhite, In.UV.xy).xyz;
    
    float _Depth = tDepthTex.Sample(sampler_point_borderWhite, In.UV.xy).x;
    
    if(_Depth <= 1.0f)
    {
        _Depth = ConvertZToLinearDepth(_Depth);
        float3 _ColorBlurred = tHDRBlurred.Sample(sampler_linear, In.UV.xy).xyz;

        _Color = DistanceDOF(_Color, _ColorBlurred, _Depth);
    }
    
    //add bloom values contribution
    _Color += BloomScale * tBloomTex.Sample(sampler_linear, In.UV.xy).xyz;

	// Tone mapping
    _Color = ToneMapping(_Color);

	// Output the LDR value
    return float4(_Color, 1.0);
}